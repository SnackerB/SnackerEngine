#include "SERP\SerpManager.h"
#include "Core\Log.h"
#include "Utility\Formatting.h"
#include "Utility\Timer.h"

#include <stdexcept>

namespace SnackerEngine
{
	
	void SERPManager::PendingResponse::onMove(PendingResponse* other)
	{
		if (serpManager) {
			auto it = serpManager->sentRequests.find(expectedMesageID);
			if (it != serpManager->sentRequests.end()) {
				auto it2 = it->second.pendingResponses.find(expectedSerpID);
				if (it2 != it->second.pendingResponses.end()) {
					if (it2->second == other) it2->second = this;
					else errorLogger << LOGGER::BEGIN << "sent request with messageID " << expectedMesageID << " and expected expectedSerpID " << expectedSerpID << " has multiple PendingResponse objects." << LOGGER::ENDL;
				}
			}
			else {
				errorLogger << LOGGER::BEGIN << "Could not find sentRequest object with messageID " << expectedMesageID << LOGGER::ENDL;
			}
		}
	}
	
	SERPManager::PendingResponse::PendingResponse(PendingResponse&& other) noexcept
		: serpManager{ other.serpManager }, status{ other.status }, response{ std::move(other.response) },
		expectedMesageID{ other.expectedMesageID }, expectedSerpID(other.expectedSerpID)
	{
		onMove(&other);
		other.serpManager = nullptr;
	}
	
	SERPManager::PendingResponse& SERPManager::PendingResponse::operator=(PendingResponse&& other) noexcept
	{
		this->serpManager = other.serpManager;
		this->status = other.status;
		this->response = std::move(other.response);
		this->expectedMesageID = other.expectedMesageID;
		this->expectedSerpID = other.expectedSerpID;
		onMove(&other);
		other.serpManager = nullptr;
		return *this;
	}
	
	SERPManager::PendingResponse::~PendingResponse()
	{
		if (serpManager) {
			// Delete this Pending response from the sentRequests!
			// Look for sentRequest with the messageID
			auto it = serpManager->sentRequests.find(expectedMesageID);
			if (it != serpManager->sentRequests.end()) {
				auto it2 = it->second.pendingResponses.find(expectedSerpID);
				if (it2 != it->second.pendingResponses.end()) {
					if (it2->second == this) it2->second = nullptr;
					else errorLogger << LOGGER::BEGIN << "sent request with messageID " << expectedMesageID << " and expected expectedSerpID " << expectedSerpID << " has multiple PendingResponse objects." << LOGGER::ENDL;
				}
			}
			else {
				errorLogger << LOGGER::BEGIN << "Could not find sentRequest object with messageID " << expectedMesageID << LOGGER::ENDL;
			}
		}
	}

	double SERPManager::PendingResponse::getTimeLeftUntilTimeout() const
	{
		if (!serpManager) return 0.0;
		// Look for sentRequest with the messageID
		auto it = serpManager->sentRequests.find(expectedMesageID);
		if (it != serpManager->sentRequests.end()) {
			return it->second.repetitions * it->second.timeBetweenResend + it->second.timeSinceLastSend;
		}
		else {
			errorLogger << LOGGER::BEGIN << "Could not find sentRequest object with messageID " << expectedMesageID << LOGGER::ENDL;
			return 0.0;
		}
	}

	bool SERPManager::checkIfAlreadyRespondedAndResend(const SERPRequest& request)
	{
		// Acquire lock
		std::lock_guard lock(sentResponsesMutex);
		// Look for sentResponse with the messageID
		auto it = sentResponses.find(request.getHeader().messageID);
		if (it != sentResponses.end()) {
			for (auto& response : it->second) {
				if (response.response->getHeader().destination == request.getHeader().source) {
					// Resend the response!
					sendResponse(*response.response);
					return true;
				}
			}
		}
		return false;
	}

	void SERPManager::insertIntoSentResponsesMap(std::shared_ptr<SERPResponse> response)
	{
		// Acquire lock
		std::lock_guard lock(sentResponsesMutex);
		// Insert response into Map
		auto it = sentResponses.find(response->getHeader().messageID);
		if (it == sentResponses.end()) {
			auto result = sentResponses.insert(std::make_pair<>(response->getHeader().messageID, std::vector<SentResponse>()));
			if (result.second) it = result.first;
			else return;
		}
		for (const auto& tempResponse : it->second) {
			if (tempResponse.response->getHeader().destination == response->getHeader().destination) return;
		}
		it->second.emplace_back(SentResponse{ sentResponsesTimeout, response });
	}

	void SERPManager::updateSentRequests(double dt)
	{
		// We start by looking if any responses were received and updating the sentRequest and PendingResponse objects accordingly
		// Acquire lock
		incomingResponsesMutex.lock();
		if (!incomingResponses.empty()) {
			// Move the queue to a temporary queue and reset the original queu
			std::queue<std::unique_ptr<SERPResponse>> tempQueue = std::move(incomingResponses);
			incomingResponses = std::queue<std::unique_ptr<SERPResponse>>();
			incomingResponsesMutex.unlock();
			// We have unlocked the mutex again, we can now process the incoming responses one by one
			while (!tempQueue.empty()) {
				if (logMessages)
					infoLogger << LOGGER::BEGIN << "Received the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(static_cast<const SERPResponse&>(*tempQueue.front())) << LOGGER::ENDL;
				auto it = sentRequests.find(tempQueue.front()->getHeader().messageID);
				if (it != sentRequests.end()) {
					auto it2 = it->second.pendingResponses.find(tempQueue.front()->getHeader().getSource());
					if (it2 != it->second.pendingResponses.end()) {
						if (it2->second) {
							it->second.request->removeDestination(tempQueue.front()->getHeader().getSource());
							it2->second->response = std::move(tempQueue.front());
							it2->second->serpManager = nullptr;
							it2->second->status = PendingResponse::Status::OBTAINED;
						}
						it->second.pendingResponses.erase(it2);
						if (it->second.pendingResponses.empty()) sentRequests.erase(it);
					}
					else {
						warningLogger << "Received response with messageID " << tempQueue.front()->getHeader().messageID << " from client " << tempQueue.front()->getHeader().source << ", valid messageID but invalid serpID, expected e.g. " << it->second.pendingResponses.begin()->first << LOGGER::ENDL;
					}
				}
				else {
					warningLogger << "Received response with messageID " << tempQueue.front()->getHeader().messageID << " from client " << tempQueue.front()->getHeader().source << " without request or after timeout!" << LOGGER::ENDL;
				}
				tempQueue.pop();
			}
		}
		else {
			incomingResponsesMutex.unlock();
		}
		// Next, we go through all remaining sentRequests and update the timer (and, if necessary, resend messages).
		{
			auto it = sentRequests.begin();
			while (it != sentRequests.end()) {
				// Advance the time
				it->second.timeSinceLastSend -= dt;
				if (it->second.timeSinceLastSend <= 0.0) {
					if (it->second.repetitions > 0) {
						// Resend message
						it->second.repetitions--;
						it->second.timeSinceLastSend += it->second.timeBetweenResend;
						sendMessage(it->second.request, false);
						if (logMessages) infoLogger << LOGGER::BEGIN << "Resent the following message at time [" << getCurrentTimeAsString() << "], " << it->second.repetitions << " repititions remaining: " << to_string(*it->second.request) << LOGGER::ENDL;
					}
					else {
						// Timeout
						auto it2 = it->second.pendingResponses.begin();
						while (it2 != it->second.pendingResponses.end()) {
							if (it2->second) {
								it2->second->status = PendingResponse::Status::TIMEOUT;
								it2->second->serpManager = nullptr;
								it2 = it->second.pendingResponses.erase(it2);
								continue;
							}
							it2++;
						}
						it = sentRequests.erase(it);
						continue;
					}
				}
				if (it->second.pendingResponses.empty()) {
					it = sentRequests.erase(it);
					continue;
				}
				it++;
			}
		}
	}

	void SERPManager::updateSentResponses(double dt)
	{
		// Acquire lock
		std::lock_guard lock(sentResponsesMutex);
		// Update all sentRequests
		auto it = sentResponses.begin();
		while (it != sentResponses.end()) {
			auto it2 = it->second.begin();
			while (it2 != it->second.end()) {
				it2->timeout -= dt;
				if (it2->timeout <= 0.0) {
					it2 = it->second.erase(it2);
				}
			}
			if (it->second.empty()) {
				it = sentResponses.erase(it);
			}
			else it++;
		}
	}

	void SERPManager::handleReceivedMessage(std::unique_ptr<SERPMessage>&& message)
	{
		if (message->getHeader().getMultiSendFlag()) {
			warningLogger << LOGGER::BEGIN << "Received message with multiple destinations!" << LOGGER::ENDL;
		}
		else {
			if (message->isRequest()) {
				handleIncomingRequest(std::move(message));
			}
			else {
				handleIncomingResponse(std::move(message));
			}
		}
	}

	void SERPManager::handleIncomingRequest(std::unique_ptr<SERPMessage>&& request)
	{
		const SERPRequest& requestRef = static_cast<const SERPRequest&>(*request);
		if (logMessages) infoLogger << LOGGER::BEGIN << "Received the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(requestRef) << LOGGER::ENDL;
		if (request->getHeader().destination != serpID) {
			warningLogger << LOGGER::BEGIN << "received incoming request with wrong destinationID!" << LOGGER::ENDL;
			sendResponse(std::move(std::make_unique<SnackerEngine::SERPResponse>(requestRef, ResponseStatusCode::BAD_REQUEST, std::move(Buffer("destinationID did not match serpID!")))));
		}
		else {
			// Sort message in the correct buffer, based on the path
			std::vector<std::string> path = SERPRequest::splitTargetPath(requestRef.target);
			if (path.empty()) {
				handleIncomingManagerRequest(std::move(request));
			}
			else {
				std::deque<std::unique_ptr<SERPRequest>>* bestResult = nullptr;
				std::string currentPath = "";
				{
					std::lock_guard lock(incomingRequestsMutex);
					for (unsigned i = 0; i < path.size(); ++i) {
						currentPath.append("/");
						currentPath.append(path[i]);
						auto it = incomingRequests.find(currentPath);
						if (it != incomingRequests.end()) {
							bestResult = &it->second;
						}
					}
					if (bestResult) {
						// Check if this request is already in the queue, in which case this is a resend and we dont need to push it to the queue again
						bool requestAlreadyInQueue = false;
						for (const auto& requestInQueue : *bestResult) {
							if (requestInQueue->getHeader().source == request->getHeader().source && requestInQueue->getHeader().messageID == request->getHeader().messageID) {
								requestAlreadyInQueue = true;
								break;
							}
						}
						if (!requestAlreadyInQueue) bestResult->push_back(std::move(std::unique_ptr<SERPRequest>(static_cast<SERPRequest*>(request.release()))));
					}
				}
				if (!bestResult) {
					handleIncomingManagerRequest(std::move(request));
				}
			}
		}
	}

	void SERPManager::handleIncomingManagerRequest(std::unique_ptr<SERPMessage>&& request)
	{
		const SERPRequest& requestRef = static_cast<const SERPRequest&>(*request);
		if (requestRef.getRequestStatusCode() == RequestStatusCode::GET && requestRef.target == "/ping") {
			sendResponse(std::move(std::make_unique<SERPResponse>(requestRef, ResponseStatusCode::OK)));
		}
		else {
			warningLogger << "received request with unknown path \"" << requestRef.target << "\"" << LOGGER::ENDL;
			sendResponse(std::move(std::make_unique<SERPResponse>(requestRef, ResponseStatusCode::NOT_FOUND, std::move(Buffer("path" + requestRef.target + " was not found!")))));
		}
	}

	void SERPManager::handleIncomingResponse(std::unique_ptr<SERPMessage>&& response)
	{
		// Acquire lock
		std::lock_guard lock(incomingResponsesMutex);
		// Push back response into incomingResponses
		incomingResponses.push(std::move(std::unique_ptr<SERPResponse>(static_cast<SERPResponse*>(response.release()))));
	}

	void SERPManager::sendMessage(std::shared_ptr<SERPMessage> message, bool setMessageID)
	{
		endpointSERP.finalizeMessage(*message, setMessageID);
		{
		std::lock_guard lock(messagesToBeSentMutex);
		messagesToBeSent.push(std::move(message));
		areMessagesToBeSent = true;
		}
		// Wake up sender thread
		senderThreadConditionVariable.notify_one();
	}

	void SERPManager::runSenderThread()
	{
		if (senderThreadIsRunning.test_and_set()) return;
		while (connected.test()) {
			std::unique_lock<std::mutex> lock(messagesToBeSentMutex);
			senderThreadConditionVariable.wait(lock);
			// Check if we are still connected
			if (!connected.test()) return;
			// We have the lock and can process the first element in the messagesToBeSent queue.
			std::shared_ptr<SnackerEngine::SERPMessage> message = nullptr;
			if (!messagesToBeSent.empty()) {
				message = std::move(messagesToBeSent.front());
				messagesToBeSent.pop();
			}
			else {
				areMessagesToBeSent = false;
			}
			// Before we send, unlock the queue again, st. other threads don't have to wait
			lock.unlock();
			// Now we can send the message
			endpointSERP.sendMessage(*message);
			// Check if there are now more messages we can send
			while (true) {
				message = nullptr;
				{
					std::lock_guard lockGuard(messagesToBeSentMutex);
					if (!messagesToBeSent.empty()) {
						message = std::move(messagesToBeSent.front());
						messagesToBeSent.pop();
					}
					else {
						areMessagesToBeSent = false;
					}
				}
				// If we are disconnected, stop the thread
				if (!connected.test()) return;
				// If the queue was empty, leave the loop
				if (!message) break;
				// Else just keep sending messages
				endpointSERP.sendMessage(*message);
			}
			while (endpointSERP.hasUnsentMessages()) endpointSERP.updateSend();
		}
		senderThreadIsRunning.clear();
	}

	void SERPManager::runReceiverThread()
	{
		if (receiverThreadIsRunning.test_and_set()) return;
		// Create poll file descriptor for listening to received messages
		pollfd incomingMesageFD(endpointSERP.getTCPEndpoint().getSocket().sock, POLLRDNORM, NULL);
		while (connected.test()) {
			// Listen for message
			int result = WSAPoll(&incomingMesageFD, 1, pollFdTimeout);
			if (result == SOCKET_ERROR) {
				// Disconnect and end thread
				connected.clear();
				break;
			}
			else if (incomingMesageFD.revents & POLLNVAL) {
				// Disconnect and end thread
				connected.clear();
				break;
			}
			else if (incomingMesageFD.revents & POLLERR) {
				// Disconnect and end thread
				connected.clear();
				break;
			}
			else if (incomingMesageFD.revents & POLLHUP) {
				// Disconnect and end thread
				connected.clear();
				break;
			}
			else if (incomingMesageFD.revents & POLLRDNORM) {
				// We received a message!
				std::optional<std::vector<std::unique_ptr<SnackerEngine::SERPMessage>>> result = std::move(endpointSERP.receiveMessages());
				if (!result.has_value()) disconnect();
				else {
					for (unsigned int i = 0; i < result.value().size(); ++i) {
						handleReceivedMessage(std::move(result.value()[i]));
					}
				}
			}
		}
		receiverThreadIsRunning.clear();
	}

	SERPManager::SERPManager()
		: endpointSERP{}, connected{}, senderThreadIsRunning{}, receiverThreadIsRunning{}, serpID{unsigned int(0)}, 
		incomingRequests{}, incomingRequestsMutex{}, incomingResponses{}, incomingResponsesMutex{}, sentRequests {}, 
		sentResponses{}, sentResponsesMutex{}, messagesToBeSent{}, messagesToBeSentMutex{}, sentResponsesTimeout{ 10.0 }, 
		senderThread{}, receiverThread{}, pollFdTimeout{ 500 }, serpIDResponse{ std::nullopt }, logMessages{ false }
	{
	}

	SERPManager::~SERPManager()
	{
		if (connected.test()) {
			warningLogger << LOGGER::BEGIN << "Please call disconnect() before calling the constructor of SERPManager()." << LOGGER::ENDL;
			disconnect();
		}
		// Wait for threads to finish
		if (senderThread.joinable()) 
			senderThread.join();
		if (receiverThread.joinable())
			receiverThread.join();
	}

	SERPManager::ConnectResult SERPManager::connectToSERPServer()
	{
		if (!connected.test()) {
			if (senderThreadIsRunning.test() || receiverThreadIsRunning.test()) {
				return { ConnectResult::Result::PENDING, "" };
			}
			if (senderThread.joinable()) senderThread.join();
			if (receiverThread.joinable()) receiverThread.join();
			auto result = endpointSERP.connectToSERPServer();
			if (result.result == SnackerEngine::ConnectResult::Result::SUCCESS) {
				connected.test_and_set();
				// Start sender and receiver thread!
				senderThread = std::thread(&SERPManager::runSenderThread, this);
				receiverThread = std::thread(&SERPManager::runReceiverThread, this);
				// Send SERPIDRequest
				serpIDResponse = std::move(sendRequest(std::move(std::make_unique<SERPRequest>(unsigned int(0), RequestStatusCode::GET, "/serpID"))));
				return { ConnectResult::Result::PENDING, "" };
			}
			else if (result.result == SnackerEngine::ConnectResult::Result::ERROR) {
				endpointSERP.resetSocket();
				return { ConnectResult::Result::ERROR, "Connecting to SERP server failed with error code " + to_string(result.error) };
			}
			else return { ConnectResult::Result::PENDING, "" };
		}
		else if (serpIDResponse) {
			switch (serpIDResponse->getStatus())
			{
			case PendingResponse::Status::OBTAINED:
			{
				if (serpIDResponse->getResponse().getResponseStatusCode() == ResponseStatusCode::OK) {
					auto result = from_string<SERPID>(serpIDResponse->getResponse().content.to_string());
					if (result.has_value()) {
						serpID = result.value();
						return { ConnectResult::Result::SUCCESS, "" };
					}
					else {
						endpointSERP.resetSocket();
						return { ConnectResult::Result::ERROR, "Connecting to SERP server failed, Server sent invalid SERPID \"" + serpIDResponse->getResponse().content.to_string() + "\"" };
					}
				}
				else {
					endpointSERP.resetSocket();
					return { ConnectResult::Result::ERROR, "Connecting to SERP server failed, Server sent invalid message." };
				}
			}
			case PendingResponse::Status::PENDING:
			{
				return { ConnectResult::Result::PENDING, "" };
			}
			case PendingResponse::Status::TIMEOUT:
			{
				endpointSERP.resetSocket();
				return { ConnectResult::Result::ERROR, "Connecting to SERP server failed, timeout while waiting for SERPID." };
			}
			default:
			{
				return { ConnectResult::Result::ERROR, "" };
			}
			}
		}
		return { ConnectResult::Result::ERROR, "" };
	}

	void SERPManager::registerPathForIncomingRequests(const std::string& path)
	{
		std::lock_guard lock(incomingRequestsMutex);
		if (incomingRequests.find(path) == incomingRequests.end())
			incomingRequests.insert(std::make_pair<>(path, std::deque<std::unique_ptr<SERPRequest>>()));
	}

	void SERPManager::removePathForIncomingRequests(const std::string& path)
	{
		std::lock_guard lock(incomingRequestsMutex);
		auto it = incomingRequests.find(path);
		if (it != incomingRequests.end()) {
			incomingRequests.erase(it);
		}
	}

	void SERPManager::clearIncomingRequestPaths()
	{
		std::lock_guard lock(incomingRequestsMutex);
		incomingRequests.clear();
	}

	bool SERPManager::areIncomingRequests(const std::string& path)
	{
		std::lock_guard lock(incomingRequestsMutex);
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) return false;
		return !it->second.empty();
	}

	std::deque<std::unique_ptr<SERPRequest>> SERPManager::getIncomingRequests(const std::string& path)
	{
		std::lock_guard lock(incomingRequestsMutex);
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) {
			throw std::out_of_range("\"" + path + "\" was not a registered path!");
		}
		std::deque<std::unique_ptr<SERPRequest>> result = std::move(it->second);
		it->second = std::deque<std::unique_ptr<SERPRequest>>();
		return result;
	}

	std::optional<SERPManager::PendingResponse> SERPManager::sendRequest(std::unique_ptr<SERPRequest>&& request, double timeout, unsigned repetitions)
	{
		if (request->getHeader().getMultiSendFlag()) {
			errorLogger << LOGGER::BEGIN << "Tried to send a request to multiple clients via sendRequest(). Use sendRequestMulti() instead." << LOGGER::ENDL;
			return std::nullopt;
		}
		std::shared_ptr<SERPRequest> request_new = std::move(request);
		request_new->getHeader().source = serpID;
		sendMessage(request_new);
		// Construct pendingResponse object
		PendingResponse pendingResponse(this, request_new->getHeader().messageID, request_new->getHeader().getDestination());
		std::unordered_map<uint16_t, PendingResponse*> tempMap;
		tempMap.insert(std::make_pair<>(request_new->getHeader().getDestination(), &pendingResponse));
		sentRequests.insert(std::make_pair<>(request_new->getHeader().messageID, SentRequest{ repetitions, timeout, timeout, request_new, std::move(tempMap) }));
		if (logMessages) infoLogger << LOGGER::BEGIN << "Sent the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(*request_new) << LOGGER::ENDL;
		return pendingResponse;
	}

	std::optional<SERPManager::PendingResponse> SERPManager::sendRequest(SERPRequest& request, double timeout, unsigned repetitions)
	{
		return std::move(sendRequest(std::move(std::make_unique<SERPRequest>(request)), timeout, repetitions));
	}

	std::vector<SERPManager::PendingResponse> SERPManager::sendRequestMulti(std::unique_ptr<SERPRequest>&& request, double timeout, unsigned repetitions)
	{
		if (!request->getHeader().getMultiSendFlag()) {
			std::optional<SERPManager::PendingResponse> pendingResponse = sendRequest(std::move(request), timeout, repetitions);
			if (pendingResponse.has_value()) {
				std::vector<PendingResponse> result;
				result.push_back(std::move(pendingResponse.value()));
				return result;
			}
		}
		else {
			std::shared_ptr<SERPRequest> request_new = std::move(request);
			request_new->getHeader().source = serpID;
			sendMessage(request_new);
			// Construct pendingResponse objects
			std::vector<PendingResponse> pendingResponses;
			pendingResponses.reserve(max(request_new->getDestinations().size(), 1));
			std::unordered_map<uint16_t, PendingResponse*> tempMap;
			for (auto destination : request_new->getDestinations()) {
				pendingResponses.emplace_back(this, request_new->getHeader().messageID, destination);
				tempMap.insert(std::make_pair<>(destination, &pendingResponses.back()));
			}
			sentRequests.insert(std::make_pair<>(request_new->getHeader().messageID, SentRequest{ repetitions, timeout, timeout, request_new, std::move(tempMap) }));	
			if (logMessages) infoLogger << LOGGER::BEGIN << "Multisent the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(*request_new) << LOGGER::ENDL;
			return pendingResponses;
		}
		return std::vector<PendingResponse>();
	}

	std::vector<SERPManager::PendingResponse> SERPManager::sendRequestMulti(SERPRequest& request, double timeout, unsigned repetitions)
	{
		return std::move(sendRequestMulti(std::move(std::make_unique<SERPRequest>(request)), timeout, repetitions));
	}

	void SERPManager::sendResponse(std::unique_ptr<SERPResponse>&& response)
	{
		std::shared_ptr<SERPResponse> response_new = std::move(response);
		response_new->getHeader().source = serpID;
		sendMessage(response_new);
		if (logMessages) infoLogger << LOGGER::BEGIN << "Sent the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(*response_new) << LOGGER::ENDL;
		insertIntoSentResponsesMap(response_new);
	}

	void SERPManager::sendResponse(SERPResponse& response)
	{
		sendResponse(std::move(std::make_unique<SERPResponse>(response)));
	}

	void SERPManager::update(double dt)
	{
		if (!connected.test()) return;
		// Update all sentRequest objects
		updateSentRequests(dt);
		// Update all sentResponse objects
		updateSentResponses(dt);
		// If there are unsent messages, wake up sender thread (should happen automatically when a message is put
		// into the messagesToBeSent queue, but problems can occur sometimes, eg. on thread startup)
		if (areMessagesToBeSent) senderThreadConditionVariable.notify_one();
	}

	void SERPManager::disconnect()
	{
		connected.clear();
		senderThreadConditionVariable.notify_one();
		for (const auto& it : sentRequests) {
			for (const auto& it2 : it.second.pendingResponses) {
				if (it2.second) it2.second->serpManager = nullptr;
			}
		}
		clearIncomingRequestPaths();
	}

}