#include "SERP\SerpManager.h"
#include "Core\Log.h"

#include <stdexcept>

namespace SnackerEngine
{

	void SERPManager::PendingResponse::onMove(PendingResponse* other)
	{
		if (serpManager) {
			auto it = serpManager->pendingResponses.find(response.getSource());
			if (it != serpManager->pendingResponses.end()) it->second = this;
			else {
				auto it2 = serpManager->outgoingRequests.find(response.getSource());
				if (it2 != serpManager->outgoingRequests.end()) {
					for (unsigned int i = 0; i < it2->second.size(); ++i) {
						if (it2->second[i].second == other) {
							it2->second[i].second = this;
							return;
						}
					}
				}
			}
		}
	}
	
	SERPManager::PendingResponse::PendingResponse(PendingResponse&& other) noexcept
		: serpManager{ std::move(other.serpManager) }, status{ std::move(other.status) }, 
		timeLeft{ std::move(other.timeLeft) }, response{ std::move(other.response) }
	{
		onMove(&other);
		other.serpManager = nullptr;
	}

	SERPManager::PendingResponse& SERPManager::PendingResponse::operator=(PendingResponse&& other) noexcept
	{
		onMove(&other);
		other.serpManager = nullptr;
		return *this;
	}

	SERPManager::PendingResponse::~PendingResponse()
	{
		if (serpManager) serpManager->pendingResponses.erase(response.getSource());
	}

	void SERPManager::handleReceivedMessage(HTTPMessage& message)
	{
		switch (message.getMessageType())
		{
		case HTTPMessage::MessageType::REQUEST:
		{
			auto serpRequest = SERPRequest::parse(static_cast<HTTPRequest&>(message));
			if (serpRequest.has_value()) {
				handleIncomingRequest(serpRequest.value());
			}
			else {
				errorLogger << LOGGER::BEGIN << "obtained HTTP request does not follow SERP format:\"\n" << 
					static_cast<HTTPRequest&>(message).toString() << "\n\"" << LOGGER::ENDL;
			}
			break;
		}
		case HTTPMessage::MessageType::RESPONSE:
		{
			auto serpResponse = SERPResponse::parse(static_cast<HTTPResponse&>(message));
			if (serpResponse.has_value()) {
				handleIncomingResponse(serpResponse.value());
			}
			else {
				errorLogger << LOGGER::BEGIN << "obtained HTTP response does not follow SERP format:\"\n" <<
					static_cast<HTTPResponse&>(message).toString() << "\n\"" << LOGGER::ENDL;
			}
			break;
		}
		default:
			break;
		}
	}

	void SERPManager::handleIncomingRequest(SERPRequest& request)
	{
		if (request.getDestination() != serpID) {
			warningLogger << LOGGER::BEGIN << "received incoming request with wrong destinationID!" << LOGGER::ENDL;
			sendResponse(SERPResponse(serpID, request.getSource(),
				HTTPResponse{ ResponseStatusCode::BAD_REQUEST, {}, "destinationID did not match serpID!" }));
		}
		else {
			// Sort message in the correct buffer, based on the path
			std::vector<std::string> path = request.request.splitPath();
			if (path.empty()) {
				handleIncomingManagerRequest(request);
			}
			std::queue<SERPRequest>* bestResult = nullptr;
			std::string currentPath = "";
			for (unsigned i = 0; i < path.size(); ++i) {
				currentPath.append(path[i]);
				auto it = incomingRequests.find(currentPath);
				if (it != incomingRequests.end()) {
					bestResult = &it->second;
				}
			}
			if (bestResult) {
				bestResult->push(request);
			}
			else {
				handleIncomingManagerRequest(request);
			}

		}
	}

	void SERPManager::handleIncomingManagerRequest(SERPRequest& request)
	{
		if (request.request.requestMethod == request.request.HTTP_GET && request.request.path == "/ping") {
			sendResponse(SERPResponse(serpID, request.getSource(),
				HTTPResponse{ ResponseStatusCode::OK, {}, "" }));
		}
		else {
			warningLogger << "received request with unknown path \"" << request.request.path << "\"" << LOGGER::ENDL;
			sendResponse(SERPResponse(serpID, request.getSource(),
				HTTPResponse{ ResponseStatusCode::NOT_FOUND, {}, "path" + request.request.path + "was not found!" }));
		}
	}

	void SERPManager::handleIncomingResponse(SERPResponse& response)
	{
		auto it = pendingResponses.find(response.getSource());
		if (it != pendingResponses.end()) {
			it->second->status = PendingResponse::Status::OBTAINED;
			it->second->response = response;
		}
		else {
			warningLogger << "Received response without request or after timeout!" << LOGGER::ENDL;
		}
	}

	SERPManager::SERPManager()
		: connected{ false }, serpID{ 0 }, httpEndpoint(SocketTCP{}, false), incomingRequests{}, pendingResponses{}, outgoingRequests{},
		serpIDResponse{ nullptr }, incomingMessageBuffer{ 4000 }
	{
		auto result = createSocketTCP();
		if (!result.has_value()) {
			errorLogger << "Was not able to create socket!" << LOGGER::ENDL;
			return;
		}
		httpEndpoint = HTTPEndpoint(std::move(result.value()), false);
	}

	SERPManager::SERPManager(SERPManager&& other) noexcept
		: connected{ std::move(other.connected) }, serpID{ std::move(other.serpID) }, httpEndpoint{ std::move(other.httpEndpoint) }, 
		incomingRequests{ std::move(other.incomingRequests) }, pendingResponses{ std::move(other.pendingResponses) }, 
		outgoingRequests{ std::move(other.outgoingRequests) }, serpIDResponse{ std::move(other.serpIDResponse) },
		incomingMessageBuffer{ std::move(other.incomingMessageBuffer) }
	{
		for (const auto& pendingResponse : pendingResponses) {
			pendingResponse.second->serpManager = this;
		}
	}

	bool SERPManager::connectToSERPServer()
	{
		bool result = connectTo(httpEndpoint.getSocket(), getSERPServerAdressTCP());
		if (result) {
			connected = true;
			serpIDResponse = std::make_unique<PendingResponse>(sendRequest(SERPRequest(0, 0, { HTTPRequest::RequestMethod::HTTP_GET, "/serpID" })));
			if (!setToNonBlocking(httpEndpoint.getSocket())) {
				errorLogger << "Was not able to set socket to non-blocking!" << LOGGER::ENDL;
				httpEndpoint.setSocket(SocketTCP{});
				return false;
			}
			return true;
		}
		errorLogger << "Was not able to connect to SERP Server!" << LOGGER::ENDL;
		return false;
	}

	void SERPManager::registerPathForIncomingRequests(const std::string& path)
	{
		if (incomingRequests.find(path) == incomingRequests.end())
			incomingRequests.insert(std::make_pair<>(path, std::queue<SERPRequest>()));
	}

	bool SERPManager::areIncomingRequests(const std::string& path)
	{
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) return false;
		return !it->second.empty();
	}

	std::queue<SERPRequest>& SERPManager::getIncomingRequests(const std::string& path)
	{
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) {
			throw std::out_of_range("\"" + path + "\" was not a registered path!");
		}
		return it->second;
	}

	SERPManager::PendingResponse SERPManager::sendRequest(SERPRequest request, double timeout)
	{
		// Construct pendingResponse object
		PendingResponse pendingResponse(this, timeout, request.getDestination());
		// Check if we are already wating for a response from the destination
		auto it = pendingResponses.find(request.getDestination());
		if (it == pendingResponses.end()) {
			// We are not currently waiting for a response from destination
			// We can therefore send the message directly!
			pendingResponses.insert(std::make_pair<>(request.getDestination(), &pendingResponse));
			request.request.addContentLengthHeader();
			SnackerEngine::sendRequest(httpEndpoint.getSocket(), request.request);
		}
		else {
			// We are currently waiting for a response from destination
			// We must push the request in the correct outgoing requests queue
			auto it2 = outgoingRequests.find(request.getDestination());
			if (it2 != outgoingRequests.end()) {
				it2->second.push_back(std::make_pair<>(request, &pendingResponse));
			}
			else {
				auto result = outgoingRequests.insert(std::make_pair<>(request.getDestination(), std::vector<std::pair<SERPRequest, PendingResponse*>>()));
				if (result.second) result.first->second.push_back(std::make_pair<>(request, &pendingResponse));
			}
		}
		return pendingResponse;
	}

	void SERPManager::sendResponse(SERPResponse response)
	{
		response.response.addContentLengthHeader();
		SnackerEngine::sendResponse(httpEndpoint.getSocket(), response.response);
	}

	void SERPManager::update(double dt)
	{
		if (!connected) return;
		// Check if we have not yet received our serpID from the server
		if (serpID == 0) {
			if (!serpIDResponse || serpIDResponse->getStatus() == PendingResponse::Status::TIMEOUT) {
				// Send another serpID request
				serpIDResponse = nullptr;
				serpIDResponse = std::make_unique<PendingResponse>(sendRequest(SERPRequest(0, 0, { HTTPRequest::RequestMethod::HTTP_GET, "/serpID" })));
			}
			else if (serpIDResponse->getStatus() == PendingResponse::Status::OBTAINED) {
				const SERPResponse& response = serpIDResponse->getResponse();
				if (response.response.responseStatusCode == ResponseStatusCode::OK) {
					auto result = parseSERPID(response.response.body);
					if (result.has_value()) {
						serpID = result.value();
					}
				}
				serpIDResponse = nullptr;
				if (serpID == 0) {
					serpIDResponse = std::make_unique<PendingResponse>(sendRequest(SERPRequest(0, 0, { HTTPRequest::RequestMethod::HTTP_GET, "/serpID" })));
				}
				else {
					infoLogger << LOGGER::BEGIN << "Connected to SERP server with serpID " << serpID << "!" << LOGGER::ENDL;
				}
			}
		}
		// Check if any new incoming messages are here
		auto receivedMessages = httpEndpoint.receiveMessages(0.01);
		for (auto& receivedMessage : receivedMessages) {
			handleReceivedMessage(*receivedMessage);
		}
		// Check if we are able to send anything from the outgoingRequest queues
		for (auto it = outgoingRequests.begin(); it != outgoingRequests.end(); ) {
			if (pendingResponses.find(it->first) == pendingResponses.end()) {
				pendingResponses.insert(std::make_pair<>(it->first, it->second.front().second));
				it->second.front().first.request.addContentLengthHeader();
				SnackerEngine::sendRequest(httpEndpoint.getSocket(), it->second.front().first.request);
				it->second.erase(it->second.begin());
				if (it->second.empty()) {
					outgoingRequests.erase(it++);
				}
				else {
					it++;
				}
			}
		}
		// Update the time of all pending responses
		for (auto& pendingResponse : pendingResponses) {
			pendingResponse.second->timeLeft -= dt;
			if (pendingResponse.second->timeLeft <= 0) {
				pendingResponse.second->timeLeft = 0;
				if (pendingResponse.second->status == PendingResponse::Status::PENDING)
					pendingResponse.second->status = PendingResponse::Status::TIMEOUT;
			}
		}
		for (auto& outgoingRequest : outgoingRequests) {
			for (auto& request : outgoingRequest.second) {
				auto& pendingResponse = request.second;
				pendingResponse->timeLeft -= dt;
				if (pendingResponse->timeLeft <= 0) {
					pendingResponse->timeLeft = 0;
					if (pendingResponse->status == PendingResponse::Status::PENDING)
						pendingResponse->status = PendingResponse::Status::TIMEOUT;
				}
			}
		}
	}

}