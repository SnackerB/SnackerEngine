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
			auto it = serpManager->sentRequests.find(response.getHeader().messageID);
			if (it != serpManager->sentRequests.end()) {
				if (it->second.pendingResponse != other)  errorLogger << LOGGER::BEGIN << "sent request with messageID " << response.getHeader().messageID << " has multiple PendingResponse objects." << LOGGER::ENDL;
				it->second.pendingResponse = this;
			}
			else {
				errorLogger << LOGGER::BEGIN << "Could not find sentRequest object with messageID " << response.getHeader().messageID << LOGGER::ENDL;
			}
		}
	}
	
	SERPManager::PendingResponse::PendingResponse(PendingResponse&& other) noexcept
		: serpManager{ other.serpManager }, status{ other.status }, response{ std::move(other.response) }
	{
		onMove(&other);
		other.serpManager = nullptr;
	}

	SERPManager::PendingResponse& SERPManager::PendingResponse::operator=(PendingResponse&& other) noexcept
	{
		serpManager = other.serpManager;
		status = other.status;
		response = std::move(other.response);
		onMove(&other);
		other.serpManager = nullptr;
		return *this;
	}

	SERPManager::PendingResponse::~PendingResponse()
	{
		if (serpManager) {
			auto it = serpManager->sentRequests.find(response.getHeader().messageID);
			if (it != serpManager->sentRequests.end()) {
				if (it->second.pendingResponse == this) it->second.pendingResponse = nullptr;
				else errorLogger << LOGGER::BEGIN << "sent request with messageID " << response.getHeader().messageID << " has multiple PendingResponse objects." << LOGGER::ENDL;
			}
			else {
				errorLogger << LOGGER::BEGIN << "Could not find sentRequest object with messageID " << response.getHeader().messageID << LOGGER::ENDL;
			}
		}
	}

	double SERPManager::PendingResponse::getTimeLeftUntilTimeout() const
	{
		if (!serpManager) return 0.0;
		auto it = serpManager->sentRequests.find(response.getHeader().messageID);
		if (it != serpManager->sentRequests.end()) {
			return it->second.repetitions * it->second.timeBetweenResend + it->second.timeSinceLastSend;
		}
		else {
			errorLogger << LOGGER::BEGIN << "Could not find sentRequest object with messageID " << response.getHeader().messageID << LOGGER::ENDL;
			return 0.0;
		}
	}

	bool SERPManager::checkIfAlreadyRespondedAndResend(const SERPRequest& request)
	{
		auto it = sentResponses.find(request.getHeader().messageID);
		if (it != sentResponses.end()) {
			for (auto& response : it->second) {
				if (response.response.getHeader().destination == request.getHeader().source) {
					// Resend the response!
					endpointSERP.sendMessage(response.response);
					return true;
				}
			}
		}
		return false;
	}

	void SERPManager::insertIntoSentResponsesMap(SERPResponse response)
	{
		auto it = sentResponses.find(response.getHeader().messageID);
		if (it == sentResponses.end()) {
			auto result = sentResponses.insert(std::make_pair<>(response.getHeader().messageID, std::vector<SentResponse>()));
			if (result.second) it = result.first;
			else return;
		}
		for (const auto& tempResponse : it->second) {
			if (tempResponse.response.getHeader().destination == response.getHeader().destination) return;
		}
		it->second.push_back(SentResponse{ sentResponsesTimeout, response });
	}

	void SERPManager::updateSentResponses(double dt)
	{
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

	void SERPManager::handleReceivedMessage(SERPMessage& message)
	{
		if (message.isRequest()) {
			handleIncomingRequest(static_cast<SERPRequest&>(message));
		}
		else {
			handleIncomingResponse(static_cast<SERPResponse&>(message));
		}
	}

	void SERPManager::handleIncomingRequest(SERPRequest& request)
	{
		if (logMessages) infoLogger << LOGGER::BEGIN << "Received the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(request) << LOGGER::ENDL;
		if (request.getHeader().destination != serpID) {
			warningLogger << LOGGER::BEGIN << "received incoming request with wrong destinationID!" << LOGGER::ENDL;
			sendResponse(SERPResponse(request, ResponseStatusCode::BAD_REQUEST, Buffer("destinationID did not match serpID!")));
		}
		else {
			// Sort message in the correct buffer, based on the path
			std::vector<std::string> path = SERPRequest::splitTargetPath(request.target);
			if (path.empty()) {
				handleIncomingManagerRequest(request);
			}
			std::deque<SERPRequest>* bestResult = nullptr;
			std::string currentPath = "";
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
					if (requestInQueue.getHeader().source == request.getHeader().source && requestInQueue.getHeader().messageID == request.getHeader().messageID) {
						requestAlreadyInQueue = true;
						break;
					}
				}
				if (!requestAlreadyInQueue) bestResult->push_back(request);
			}
			else {
				handleIncomingManagerRequest(request);
			}

		}
	}

	void SERPManager::handleIncomingManagerRequest(SERPRequest& request)
	{
		if (request.getRequestStatusCode() == RequestStatusCode::GET && request.target == "/ping") {
			sendResponse(SERPResponse(request, ResponseStatusCode::OK));
		}
		else {
			warningLogger << "received request with unknown path \"" << request.target << "\"" << LOGGER::ENDL;
			sendResponse(SERPResponse(request, ResponseStatusCode::NOT_FOUND, Buffer("path" + request.target + " was not found!")));
		}
	}

	void SERPManager::handleIncomingResponse(SERPResponse& response)
	{
		if (logMessages) infoLogger << LOGGER::BEGIN << "Received the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(response) << LOGGER::ENDL;
		auto it = sentRequests.find(response.header.messageID);
		if (it != sentRequests.end()) {
			if (it->second.pendingResponse) {
				it->second.pendingResponse->status = PendingResponse::Status::OBTAINED;
				it->second.pendingResponse->response = response;
				it->second.pendingResponse->serpManager = nullptr;
			}
			sentRequests.erase(it);
		}
		else {
			warningLogger << "Received response with messageID " << response.header.messageID << " without request or after timeout!" << LOGGER::ENDL;
		}
	}

	SERPManager::SERPManager()
		: endpointSERP{}, connected{ false }, serpID{ 0 }, incomingRequests{}, sentRequests{}, sentResponses{}, sentResponsesTimeout{ 10.0 }, serpIDResponse { nullptr }, logMessages{ false }
	{
	}

	SERPManager::~SERPManager()
	{
		for (const auto& it : sentRequests) {
			if (it.second.pendingResponse) it.second.pendingResponse->serpManager = nullptr;
		}
		clearIncomingRequestPaths();
	}

	SERPManager::SERPManager(SERPManager&& other) noexcept
		: endpointSERP{ std::move(other.endpointSERP) }, connected{ std::move(other.connected) }, serpID{ std::move(other.serpID) },
		incomingRequests{ std::move(other.incomingRequests) }, sentRequests{ std::move(other.sentRequests) }, sentResponses{ std::move(other.sentResponses) }, 
		sentResponsesTimeout{ other.sentResponsesTimeout }, serpIDResponse { std::move(other.serpIDResponse) }, logMessages{ other.logMessages }
	{
		for (const auto& it : sentRequests) {
			if (it.second.pendingResponse) it.second.pendingResponse->serpManager = this;
		}
		other.sentRequests.clear();
	}

	SERPManager& SERPManager::operator=(SERPManager&& other) noexcept
	{
		for (const auto& it : sentRequests) {
			if (it.second.pendingResponse) it.second.pendingResponse->serpManager = nullptr;
		}
		endpointSERP = std::move(other.endpointSERP);
		connected = other.connected;
		serpID = other.serpID;
		incomingRequests = std::move(other.incomingRequests);
		sentRequests = std::move(other.sentRequests);
		serpIDResponse = std::move(other.serpIDResponse);
		logMessages = other.logMessages;
		for (const auto& it : sentRequests) {
			if (it.second.pendingResponse) it.second.pendingResponse->serpManager = this;
		}
		other.sentRequests.clear();
		return *this;
	}

	SERPManager::ConnectResult SERPManager::connectToSERPServer()
	{
		if (!connected) {
			auto result = endpointSERP.connectToSERPServer();
			if (result.result == SnackerEngine::ConnectResult::Result::SUCCESS) {
				connected = true;
				serpIDResponse = std::make_unique<PendingResponse>(sendRequest(SERPRequest(0, RequestStatusCode::GET, "/serpID")));
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
						return { ConnectResult::Result::PENDING, "" };
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
				break;
			}
			case PendingResponse::Status::PENDING:
			{
				return { ConnectResult::Result::PENDING, "" };
				break;
			}
			case PendingResponse::Status::TIMEOUT:
			{
				endpointSERP.resetSocket();
				return { ConnectResult::Result::ERROR, "Connecting to SERP server failed, timeout while waiting for SERPID." };
				break;
			}
			}
		}
	}

	void SERPManager::registerPathForIncomingRequests(const std::string& path)
	{
		if (incomingRequests.find(path) == incomingRequests.end())
			incomingRequests.insert(std::make_pair<>(path, std::deque<SERPRequest>()));
	}

	void SERPManager::removePathForIncomingRequests(const std::string& path)
	{
		auto it = incomingRequests.find(path);
		if (it != incomingRequests.end()) {
			incomingRequests.erase(it);
		}
	}

	void SERPManager::clearIncomingRequestPaths()
	{
		incomingRequests.clear();
	}

	bool SERPManager::areIncomingRequests(const std::string& path)
	{
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) return false;
		return !it->second.empty();
	}

	std::deque<SERPRequest>& SERPManager::getIncomingRequests(const std::string& path)
	{
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) {
			throw std::out_of_range("\"" + path + "\" was not a registered path!");
		}
		return it->second;
	}

	SERPManager::PendingResponse SERPManager::sendRequest(SERPRequest request, double timeout, unsigned repetitions)
	{
		// Construct pendingResponse object
		request.getHeader().source = serpID;
		endpointSERP.sendMessage(request);
		PendingResponse pendingResponse(this, request.getHeader().messageID);
		auto it = sentRequests.insert(std::make_pair<>(request.getHeader().messageID, SentRequest{ repetitions, timeout, timeout, request, &pendingResponse }));
		if (logMessages) infoLogger << LOGGER::BEGIN << "Sent the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(request) << LOGGER::ENDL;
		return pendingResponse;
	}

	void SERPManager::sendResponse(SERPResponse response)
	{
		response.getHeader().source = serpID;
		endpointSERP.sendMessage(response);
		if (logMessages) infoLogger << LOGGER::BEGIN << "Sent the following message at time [" << getCurrentTimeAsString() << "]: " << to_string(response) << LOGGER::ENDL;
		insertIntoSentResponsesMap(response);
	}

	void SERPManager::update(double dt)
	{
		if (!connected) return;
		// Check if any new incoming messages are here
		auto receivedMessages = endpointSERP.receiveMessages();
		for (auto& receivedMessage : receivedMessages) {
			handleReceivedMessage(*receivedMessage);
		}
		// Update all sentRequest objects
		auto it = sentRequests.begin();
		while (it != sentRequests.end()) {
			// Advance the time
			it->second.timeSinceLastSend -= dt;
			if (it->second.timeSinceLastSend <= 0.0) {
				if (it->second.repetitions > 0) {
					// Resend message
					it->second.repetitions--;
					it->second.timeSinceLastSend += it->second.timeBetweenResend;
					endpointSERP.sendMessage(it->second.request, false);
					if (logMessages) infoLogger << LOGGER::BEGIN << "Resent the following message at time [" << getCurrentTimeAsString() << "], " << it->second.repetitions << " repititions remaining: " << to_string(it->second.request) << LOGGER::ENDL;
				}
				else {
					// Timeout
					if (it->second.pendingResponse) {
						it->second.pendingResponse->status = PendingResponse::Status::TIMEOUT;
						it->second.pendingResponse->serpManager = nullptr;
						it = sentRequests.erase(it);
						continue;
					}
				}
			}
			it++;
		}
		// Update all sentResponse objects
		updateSentResponses(dt);
	}

}