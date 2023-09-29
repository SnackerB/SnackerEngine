#pragma once

#include "SERP\SERP.h"
#include "TCP/TCP.h"

#include <unordered_map>
#include <string>
#include <queue>

namespace SnackerEngine
{

	/// A SERPManager represents the Engines connection with the network. It is used to communicate with other clients 
	/// running SnackerEngine over the SERP server
	class SERPManager
	{
	public:
		/// Class that represents a pending response. An object of this class is returned by the networkmanager for every request made.
		/// If the request is answered or times out, the pendingResponse object is notified, and the potential response can be obtained.
		/// Upon destruction of the pendingResponse object, the networkManager removes the reference to the object from the
		/// pendingResponses map.
		class PendingResponse
		{
		public:
			/// Tristate enum representing the state of the response
			enum class Status {
				PENDING,	/// Response is still pending
				TIMEOUT,	/// Response timed out
				OBTAINED,	/// Response was obtained and can be requested from the SERPManager
			};
		private:
			friend class SERPManager;
			/// The SERPManager that takes care and updates this pendingResponse object
			SERPManager* serpManager;
			/// Current status
			Status status;
			/// Time left until timeout
			double timeLeft;
			/// The actual response (if it is obtained).
			/// This field holds the expected sourceID even before the message is obtained
			SnackerNet::SERPResponse response;
			/// private constructor, is only called by SERPManager!
			PendingResponse(SERPManager* serpManager, double timeLeft, SnackerNet::SERPID sourceID)
				: serpManager{ serpManager }, status{ Status::PENDING }, timeLeft{ timeLeft }, response{} 
			{
				response.source = sourceID;
			}
			/// Helper function that should be called on move
			void onMove(PendingResponse* other);
		public:
			/// Deleted copy constructor and assignment operator
			PendingResponse(const PendingResponse& other) = delete;
			PendingResponse& operator=(const PendingResponse& other) = delete;
			/// Move constructor and assignment operator
			PendingResponse(PendingResponse&& other) noexcept;
			PendingResponse& operator=(PendingResponse&& other) noexcept;
			/// Destructor
			~PendingResponse();
			/// Getters
			Status getStatus() const { return status; }
			double getTimeLeft() const { return timeLeft; }
			const SnackerNet::SERPResponse& getResponse() const { return response; }
			/// Setters
			void setTimeLeft(double timeLeft) { this->timeLeft = timeLeft; }
		};
	private:
		/// Wether the SERP manager is connected to the SERP server
		bool connected = false;
		/// The serpID of this SERPManager. If this is set to zero, no serpID has been broadcasted yet.
		SnackerNet::SERPID serpID = 0;
		/// The socket that is used to communicate with the server
		SnackerNet::SocketTCP socket{};
		/// Queues of incoming requests. User can register different paths that are listened to. An incoming request is
		/// pushed into the queue with the most specialized path matching the request path. If there is no matching path,
		/// the request is answered with an error message
		std::unordered_map<std::string, std::queue<SnackerNet::SERPRequest>> incomingRequests{};
		/// Map of all requests that were sent and have not yet received an answer. This map is used to map incoming responses to these requests
		std::unordered_map<SnackerNet::SERPID, PendingResponse*> pendingResponses{};
		/// Queues of outgoing requests. Is used as a backup if multiple requests are sent to the same client without waiting for an answer or
		/// timeout first.
		std::unordered_map<SnackerNet::SERPID, std::vector<std::pair<SnackerNet::SERPRequest, PendingResponse*>>> outgoingRequests{};
		/// Request for serpID, which is sent after connecting to the SERPServer
		std::unique_ptr<PendingResponse> serpIDResponse = nullptr;
		/// Buffer for incoming messages
		SnackerNet::Buffer incomingMessageBuffer{ 0 };
		/// Helper function handling a received message
		void handleReceivedMessage(SnackerNet::Buffer& buffer);
		/// Helper function handling incoming serpRequest
		void handleIncomingRequest(SnackerNet::SERPRequest request);
		/// Helper function handling incoming serpRequest addressed to the manager (eg. ping request)
		void handleIncomingManagerRequest(SnackerNet::SERPRequest request);
		/// Helper function handling incoming serpResponse
		void handleIncomingResponse(SnackerNet::SERPResponse response);
	public:
		/// Constructor
		SERPManager();
		/// Deleted copy constructor and assignment operator
		SERPManager(const SERPManager& other) = delete;
		SERPManager& operator=(const SERPManager& other) = delete;
		/// Move constructor and assignment operator
		SERPManager(SERPManager&& other) noexcept;
		SERPManager& operator=(SERPManager&& other) = delete;
		/// Tries to connect to the SERP server
		bool connectToSERPServer();
		/// Registers a path to listen to for incoming requests
		void registerPathForIncomingRequests(const std::string& path);
		/// Returns true if the given path was registered and there are requests in the corresponding queue
		bool areIncomingRequests(const std::string& path);
		/// Returns reference to the incoming requests queue for the given path. This requires the given path
		/// to be registered before. If the given path was not registered, this function will raise an exception.
		/// To be prevent this, consider calling areIncomingRequests(path) before to check if any requests are present
		/// under the given path. Good practice: After calling this function, answer all requests and clear the queue!
		std::queue<SnackerNet::SERPRequest>& getIncomingRequests(const std::string& path);
		/// Sends a request to the client with the given serpID and returns a PendingResponse object that can be used to check
		/// the response or detect a timeout. Note that no other request can be sent to the client with the given SERPID, until the
		/// original request either times out or is answered. Further requests to the same clients are instead placed in a queue and 
		/// sent out later.
		PendingResponse sendRequest(SnackerNet::SERPRequest request, double timeout = 0.5);
		/// Sends a SERPResponse
		void sendResponse(SnackerNet::SERPResponse response);
		/// updates the SERPManager and checks for incoming messages. This should be called frequently during runtime
		/// to ensure a stable connection. Consider running this on a specialized thread.
		void update(double dt);
	};

}