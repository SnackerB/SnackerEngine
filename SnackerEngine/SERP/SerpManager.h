#pragma once

#include "Network\SERP\SERPEndpoint.h"

#include <unordered_map>
#include <string>
#include <queue>
#include <optional>
#include <thread>
#include <mutex>

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
			/// The actual response (if it is obtained).
			std::unique_ptr<SERPResponse> response;
			/// The expected messageID
			std::size_t expectedMesageID;
			/// Helper function that should be called on move
			void onMove(PendingResponse* other);
		public:
			/// Constructor should only be called by SERPManager!
			PendingResponse(SERPManager* serpManager, std::size_t messageID)
				: serpManager{ serpManager }, status{ Status::PENDING }, response(nullptr), expectedMesageID(messageID) {}
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
			double getTimeLeftUntilTimeout() const;
			/// Returns a reference to the response. This should ONLY be called when getStatus() returns
			/// Status::OBTAINED to avoid race conditions.
			const SERPResponse& getResponse() const { return *response; }
		};
	private:
		/// Class that represents a sent request. Each request that is sent by this SERPManager produces a SentRequest entry in the
		/// SentRequest map. If no response is obtained after a certain time, the request is sent again, up to a given number of 
		/// repetitions. Per default, the request is sent only once, but an arbitrary amount of repetitions can be chosen as an implementation
		/// of "safe search". If after all repetitions still no response was obtained, the message is considered as timeout.
		/// After a response is obtained or a timeout occurs, the corresponding PendingResponse object is notified, if it exists.
		struct SentRequest
		{
			unsigned repetitions;
			double timeBetweenResend;
			double timeSinceLastSend;
			std::shared_ptr<SERPRequest> request;
			PendingResponse* pendingResponse;
		};
		/// Class that represents a sent response. Each response that is sent by this SERPManager produces a SentResponse entry in the
		/// SentResponses map. If the response does not arrive at the destination and the other client resends the same request again,
		/// we first check the SentResponses map and just resend. After some time, responses are cleared again from the SentResponses map.
		struct SentResponse
		{
			double timeout;
			std::shared_ptr<SERPResponse> response;
		};
		/// The SERP endpoint
		SERPEndpoint endpointSERP;
		/// Wether the SERP manager is connected to the SERP server
		std::atomic<bool> connected;
		/// The serpID of this SERPManager. If this is set to zero, no serpID has been broadcasted yet.
		SERPID serpID;
		/// Queues of incoming requests. User can register different paths that are listened to. An incoming request is
		/// pushed into the queue with the most specialized path matching the request path. If there is no matching path,
		/// the request is answered with an error message. The mutex is for thread-safe access.
		std::unordered_map<std::string, std::deque<std::unique_ptr<SERPRequest>>> incomingRequests;
		std::mutex incomingRequestsMutex;
		/// Queue of incoming responses. Should be checked regularly using the update() function, which also updates the
		/// PendingResponse objects!
		std::queue<std::unique_ptr<SERPResponse>> incomingResponses;
		std::mutex incomingResponsesMutex;
		/// Map of all requests that were sent and have not yet received an answer. This map is used to map incoming responses to these requests.
		/// The key is the messageID of the requests. This map is only accessed by the main thread, so we don't need a mutex.
		std::unordered_map<std::size_t, SentRequest> sentRequests;
		/// Map of all responses that were sent in recent past. This map is used to quickly resend responses if they got lost somewhere and the other
		/// client is resending the request. The key to the map is the message id of the request, for each messageID we store a vector for responses
		/// to different clients.
		std::unordered_map<std::uint32_t, std::vector<SentResponse>> sentResponses;
		std::mutex sentResponsesMutex;
		/// Queue containing messages to be sent.
		std::queue<std::shared_ptr<SERPMessage>> messagesToBeSent;
		std::mutex messagesToBeSentMutex;
		/// Duration for how long sent responses are stored in the sentResponses map, in seconds.
		double sentResponsesTimeout;
		/// Sender and receiver threads
		std::thread senderThread;
		std::condition_variable senderThreadConditionVariable;
		std::thread receiverThread;
		/// Timeout in ms for poll file descriptors
		unsigned pollFdTimeout;
		/// Helper function that checks if a response to the given request was already sent, and if so, resends the response and returns true.
		/// Otherwise, false is returned
		bool checkIfAlreadyRespondedAndResend(const SERPRequest& request);
		/// Inserts the given response into the sentResponses map
		void insertIntoSentResponsesMap(std::shared_ptr<SERPResponse> response);
		/// Helper function going through the incomingResponses queue and updating PendingResponse objects
		/// and additionally going through all sentRequests and updating timers.
		void updateSentRequests(double dt);
		/// Updates the timer of all responses in the sentResponses map and deletes them if necessary
		void updateSentResponses(double dt);
		/// Request for serpID, which is sent after connecting to the SERPServer
		std::optional<PendingResponse> serpIDResponse;
		/// If this is set to true, every incoming and outgoing message is logged
		bool logMessages;
		/// Helper function handling a received message
		void handleReceivedMessage(std::unique_ptr<SERPMessage>&& message);
		/// Helper function handling incoming serpRequest
		void handleIncomingRequest(std::unique_ptr<SERPMessage>&& request);
		/// Helper function handling incoming serpRequest addressed to the manager (eg. ping request)
		void handleIncomingManagerRequest(std::unique_ptr<SERPMessage>&& request);
		/// Helper function handling incoming serpResponse
		void handleIncomingResponse(std::unique_ptr<SERPMessage>&& response);
		/// Helper function putting a message into the messagesToBeSent queue and wakes up the sender thread.
		void sendMessage(std::shared_ptr<SERPMessage> message, bool setMessageID = true);
		/// Checks the outgoing message queue and sends messages
		void runSenderThread();
		/// Receives and handles incoming messages.
		void runReceiverThread();
	public:
		/// Constructor
		SERPManager();
		/// Destructor
		~SERPManager();
		/// Deleted copy constructor and assignment operator
		SERPManager(const SERPManager& other) = delete;
		SERPManager& operator=(const SERPManager& other) = delete;
		/// Move constructor and assignment operator
		SERPManager(SERPManager&& other) = delete;
		SERPManager& operator=(SERPManager&& other) = delete;
		/// Struct that gets returned when calling connectToSERPServer()
		struct ConnectResult
		{
			enum class Result
			{
				SUCCESS,
				ERROR,
				PENDING,
			};
			Result result;
			std::string error;
		};
		/// Tries to connect to the SERP server
		ConnectResult connectToSERPServer();
		/// Registers a path to listen to for incoming requests
		void registerPathForIncomingRequests(const std::string& path);
		/// Removes the given path from the incoming request paths
		void removePathForIncomingRequests(const std::string& path);
		/// Clears all paths for incoming requests
		void clearIncomingRequestPaths();
		/// Returns true if the given path was registered and there are requests in the corresponding queue
		bool areIncomingRequests(const std::string& path);
		/// Returns incoming requests queue for the given path. This requires the given path
		/// to be registered before. If the given path was not registered, this function will raise an exception.
		/// To be prevent this, consider calling areIncomingRequests(path) before to check if any requests are present
		/// under the given path.
		std::deque<std::unique_ptr<SERPRequest>> getIncomingRequests(const std::string& path);
		/// Sends a request to the client with the given serpID and returns a PendingResponse object that can be used to check
		/// the response or detect a timeout. Note that no other request can be sent to the client with the given SERPID, until the
		/// original request either times out or is answered. Further requests to the same clients are instead placed in a queue and 
		/// sent out later.
		PendingResponse sendRequest(std::unique_ptr<SERPRequest>&& request, double timeout = 0.5, unsigned repetitions = 1);
		PendingResponse sendRequest(SERPRequest& request, double timeout = 0.5, unsigned repetitions = 1);
		/// Sends a SERPResponse. If possible, use the first definition of the function, as the second definition makes an
		/// additional copy of the response.
		void sendResponse(std::unique_ptr<SERPResponse>&& response);
		void sendResponse(SERPResponse& response);
		/// updates the SERPManager and checks for incoming messages. This should be called frequently during runtime
		/// to ensure a stable connection. Consider running this on a specialized thread.
		void update(double dt);
		/// Disconnects from the SERP Server (if connected). This should be called before deleting the SERPManager.
		void disconnect();
		/// Getters
		bool isConnectedToSERPServer() const { return serpID > 0; }
		SERPID getSerpID() const { return serpID; }
		bool isLogMessages() const { return logMessages; }
		/// Setters
		void setLogMessages(bool logMessages) { this->logMessages = logMessages; }
	};

}