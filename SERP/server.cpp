#include "server.h"
#include "network.h"
#include "SERP.h"
#include "SMP.h"

#include <csignal>
#include <fstream>
#include <unistd.h>
#include <memory>
#include <iomanip>
#include <ctime>
#include <unordered_map>
#include <cstring>
#include <random>
#include <chrono>
#include <cerrno>

#define SLEEP_DURATION 3 /// Duration the server sleeps for between updates when its in the sleeping state.
#define SLEEP_TIMEOUT 180 /// Time after which the server falls asleep
#define MAX_CLIENTS 50 /// Maximal amount of clients that are allowed to be registered at the same time.
#define CLIENT_TIMEOUT_SECONDS 60 /// Duration in seconds until a client times out
#define PROCESS_CLIENT_TIMEOUT_EVERY_SECONDS 1 /// Controls how often client timeouts are updated/processed.
#define SEND_PING_TO_CLIENT_AFTER_SECONDS 30 /// After hearing nothing from a client for this amount of time,
                                             /// ping requests are sent!
#define MAX_MESSAGE_LEN 4000 /// Maximal message length (all headers + payloads) in bytes
#define SERP_SERVER_PORT 33333 /// Port number of SERP
#define SOCKET_TIMEOUT_SECONDS 5 /// How long the socket will wait in a recv call before timing out.
#define CLIENT_ID_GENERATE_TRIES 10 /// How often generateNewClientID() will try.

/// Struct for saving clients
struct Client
{
    int timeout;        /// When this reaches zero, the client is kicked from the server due to inactivity.
    uint16_t id;        /// Unique id for every client.
    sockaddr_in addr;   /// address of the client, used to send messages and identify incoming messages.
    Client()
        : timeout(0), id(0), addr{} {}
    Client(const uint16_t& id, const sockaddr_in& addr)
        : timeout(0), id(id), addr(addr) {}
};

/// This bool decides if the server is running. Used to clean up properly when SIGTERM is received
volatile static bool running = false;
/// This bool decides if the server is active or sleeping. When sleeping, the server consumes a lot less resources,
/// Since it idles for SLEEP_DURATION seconds before checking if any packets were received.
static bool sleeping = false;
/// File descriptor for logging
static std::unique_ptr<std::ofstream> logger = nullptr;
/// Map of clients
std::unordered_map<uint16_t, Client> clients{};
/// Buffer, used for sending and receiving messages
std::array<char, MAX_MESSAGE_LEN> buffer{};
/// The socket of the server
int serverSocket = 0;
/// The ID for the next message sent from the server
uint32_t nextMessageID = 0;
/// Random number generator for client ids
static std::unique_ptr<std::default_random_engine> rng;
/// Last remote Address
sockaddr_in lastRemoteAddr{};

template<typename T>
void log(const T& message)
{
    if (logger && logger->is_open()) {
        *logger << message << std::endl;
    }
}

template<typename T>
void logWithoutNewline(const T& message)
{
    if (logger && logger->is_open()) {
        *logger << message;
    }
}

template<typename T, typename... Args>
void logWithoutNewline(const T& message, Args... args) // recursive variadic function
{
    if (logger && logger->is_open()) {
        *logger << message;
        logWithoutNewline(args...);
    }
}

template<typename T, typename... Args>
void log(const T& message, Args... args) // recursive variadic function
{
    if (logger && logger->is_open()) {
        *logger << message;
        logWithoutNewline(args...);
        *logger << std::endl;
    }
}

std::string getCurrentTime()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss{};
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    return oss.str();
}

unsigned int getNextMessageID()
{
    return nextMessageID++;
}

/// Tries CLIENT_ID_GENERATE_TRIES times to generate a new client id and returns it if successful.
std::optional<uint16_t> getNewClientID()
{
    std::uniform_int_distribution<uint16_t> dist(1);
    for (unsigned int i = 0; i < CLIENT_ID_GENERATE_TRIES; ++i)
    {
        uint16_t id = dist(*rng);
        if (clients.find(id) == clients.end()) return id;
    }
    return {};
}

void handleSignal(int signal)
{
    if (signal == SIGTERM) running = false;
}

/// Extracts the SERP header from the beginning of the buffer
SERP_Header extractSERPHeader()
{
    SERP_Header result{};
    std::memcpy(&result, buffer.data(), sizeof(SERP_Header));
    result.turnToHostByteOrder();
    return result;
}

/// Extracts the SMP header from the buffer
SMP_Header extractSMPHeader()
{
    SMP_Header result{};
    std::memcpy(&result, buffer.data() + sizeof(SERP_Header), sizeof(SMP_Header));
    result.turnToHostByteOrder();
    return result;
}

/// Extracts the data as a string.
std::string extractDataAsString(uint16_t messageLen)
{
    // We don't need to transform to host order, since it's an array of chars!
    return  {buffer.data() + sizeof(SERP_Header) + sizeof(SMP_Header), messageLen};
}

/// Compares two structs of type sockaddr_in
bool compareAddresses(const sockaddr_in& a, const sockaddr_in& b)
{
    return a.sin_addr.s_addr == b.sin_addr.s_addr &&
           a.sin_port == b.sin_port &&
           a.sin_family == b.sin_family;
}

/// Send a message to a given address,
/// remoteAddr: destination address
/// messageLen: length of the message in bytes
static void sendMessage(const sockaddr_in& remoteAddr, unsigned int messageLen)
{
    auto result = sendMessage(serverSocket, remoteAddr, reinterpret_cast<void*>(buffer.data()), std::min(static_cast<int>(messageLen), MAX_MESSAGE_LEN));
    if (!result.has_value()) {
        log("[ERROR]: send() failed with error code ", errno);
        log("[ERROR]: tried to send to address ", remoteAddr.sin_addr.s_addr, " : ", remoteAddr.sin_port);
    }
    else if (result.value() != std::min(static_cast<int>(messageLen), MAX_MESSAGE_LEN)) {
        log("[ERROR] Tried to send message of length", std::min(static_cast<int>(messageLen), MAX_MESSAGE_LEN), ", but only message of length ", result.value(), " was sent!");
    }
}

/// Send a message to a given client.
/// clientID: id of the client
/// messageLen: length of the message in bytes
static void sendMessage(uint16_t clientID, unsigned int messageLen)
{
    auto it = clients.find(clientID);
    if (it == clients.end()) {
        log("[ERROR]: Tried to send message to ", clientID, " but no client with this ID was registered.");
        return;
    }
    sendMessage(it->second.addr, messageLen);
}

/// Receives a message. If a message was received, returns a pair of the SERP_Header and the length of the
/// message in bytes. If an error occurs of no message was received (timeout), returns an empty optional instead.
static std::optional<std::pair<SERP_Header, uint16_t>> receiveMessage()
{
    sockaddr remoteAddr{};
    unsigned int remoteAddrLen = sizeof(sockaddr);
    auto result = receiveMessage(serverSocket, remoteAddr, remoteAddrLen, static_cast<void*>(buffer.data()), MAX_MESSAGE_LEN);
    if (!result.has_value()) {
        log ("[ERROR]: an error occurred while receiving message.");
        return {};
    }
    if (result.value() == 0) {
        return {};
    }
    if (remoteAddrLen != sizeof(sockaddr_in)) {
        log("[ERROR]: Sender had sockaddr of unknown size.");
        return {};
    }
    // Copy remoteAddr to lastRemoteAddr
    lastRemoteAddr = *((sockaddr_in*)(&remoteAddr));
    SERP_Header serpHeader = extractSERPHeader();
    auto it = clients.find(serpHeader.src);
    if (it != clients.end() && !compareAddresses(it->second.addr, *reinterpret_cast<sockaddr_in*>(&remoteAddr))) {
        log("[ERROR]: Received message from impostor!");
        return {};
    }
    return std::make_pair(serpHeader, static_cast<uint16_t>(result.value()));
}

/// Copies a SERP_Header into the buffer
static void setSERPHeader(SERP_Header serpHeader)
{
    serpHeader.turnToNetworkByteOrder();
    std::memcpy(buffer.data(), &serpHeader, sizeof(SERP_Header));
}

/// Copies a SMP_Header into the buffer (at the correct location)
static void setSMPHeader(SMP_Header smpHeader)
{
    smpHeader.turnToNetworkByteOrder();
    std::memcpy(buffer.data() + sizeof(SERP_Header), &smpHeader, sizeof(SMP_Header));
}

template<typename T>
static bool setData(const T& data)
{
    if (MAX_MESSAGE_LEN - sizeof(SERP_Header) - sizeof(SMP_Header) - sizeof(T) < 0) {
        return false;
    }
    std::memcpy(reinterpret_cast<void*>(buffer.data() + sizeof(SERP_Header) + sizeof(SMP_Header)), &data, sizeof(T));
    return true;
}

template<typename T, typename... Args>
static bool setData(const T& data, Args... args);

/// Sets the headers and the data in a way where an error message can be sent. Returns the message length.
uint16_t setErrorMessage(const uint16_t& dst, const MESSAGE_OPTION_ERROR& option, const std::string& message = "")
{
    uint16_t len = sizeof(SERP_Header) + sizeof(SMP_Header) + message.size();
    setSERPHeader(SERP_Header(0, dst, len, 0, 1, getNextMessageID()));
    setSMPHeader(SMP_Header(MESSAGE_TYPE::ERROR, option));
    if (!message.empty()) setData(message.c_str());
    return len;
}

/// This hidden namespace implements setData() as a recursive variadic function
namespace
{
    template<typename T>
    bool setDataInternal(const unsigned int& remainingLength, const T& data)
    {
        if (remainingLength - sizeof(T) < 0) {
            return false;
        }
        std::memcpy(buffer.data() + MAX_MESSAGE_LEN - remainingLength, &data, sizeof(T));
        return true;
    }

    template<typename T, typename... Args>
    bool setDataInternal(const unsigned int& remainingLength, const T& data, Args... args) // recursive variadic function
    {
        if (remainingLength - sizeof(T) < 0) {
            return false;
        }
        std::memcpy(buffer.data() + MAX_MESSAGE_LEN - remainingLength, &data, sizeof(T));
        return setDataInternal(remainingLength - sizeof(T), args...);
    }

    template<typename T, typename... Args>
    bool setData(const T& data, Args... args) // recursive variadic function
    {
        if (MAX_MESSAGE_LEN - sizeof(SERP_Header) - sizeof(SMP_Header) - sizeof(T) < 0) {
            return false;
        }
        std::memcpy(buffer.data() + sizeof(SERP_Header) + sizeof(SMP_Header), &data, sizeof(T));
        return setDataInternal(MAX_MESSAGE_LEN - sizeof(SERP_Header) - sizeof(SMP_Header) - sizeof(T), args...);
    }
}

/// Connects a new client to the server, and sends the client its id. If the server is full an error message is sent
/// to the client instead. Returns true if the client successfully connected.
static bool connectClient(const sockaddr_in& addr)
{
    auto clientID = getNewClientID();
    if (!clientID.has_value()) {
        uint16_t len = setErrorMessage(0, MESSAGE_OPTION_ERROR::UNSPECIFIED, "Could not create new client id!");
        sendMessage(addr, len);
        return false;
    }
    if (clients.size() >= MAX_CLIENTS) {
        uint16_t len = setErrorMessage(0, MESSAGE_OPTION_ERROR::TOO_MANY_CLIENTS);
        sendMessage(addr, len);
        return false;
    }
    clients.insert(std::make_pair(clientID.value(), Client(clientID.value(), addr)));
    uint16_t messageLen = sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(uint16_t);
    setSERPHeader(SERP_Header(0, clientID.value(), messageLen, 0, 1, getNextMessageID()));
    setSMPHeader(SMP_Header(MESSAGE_TYPE::ADVERTISEMENT, MESSAGE_OPTION_ADVERTISEMENT::OK));
    setData(htons(clientID.value()));
    sendMessage(clientID.value(), messageLen);
    log("New client connected. ID: ", clientID.value(), ", address: ", addr.sin_addr.s_addr, ":", addr.sin_port);
    return true;
}

/// Disconnects client. Does not notify client.
static void disconnectClient(uint16_t clientID)
{
    auto it = clients.find(clientID);
    if (it == clients.end()) {
        log("[ERROR]: Tried to disconnect client that was not registered!");
        return;
    }
    clients.erase(it);
    log("client disconnected. ID: ", clientID);
}

/// Handles an advertisement message that was sent addressed to this server
static void handleAdvertisement(const SERP_Header& serpHeader, MESSAGE_OPTION_ADVERTISEMENT messageOption)
{
    switch (messageOption)
    {
        case MESSAGE_OPTION_ADVERTISEMENT::REQUEST:
        {
            auto it = clients.find(serpHeader.src);
            if (it != clients.end()) {
                // Client was already registered ... just send it the same id again
                unsigned int messageLen = sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(int16_t);
                setSERPHeader(SERP_Header(0, serpHeader.src, static_cast<uint16_t>(messageLen), 0, 1, getNextMessageID()));
                setSMPHeader(SMP_Header(MESSAGE_TYPE::ADVERTISEMENT, MESSAGE_OPTION_ADVERTISEMENT::OK));
                setData(htons(serpHeader.src));
                sendMessage(serpHeader.src, messageLen);
            }
            else {
                // Client was not yet registered: Connect client!
                connectClient(lastRemoteAddr);
            }
            break;
        }
        case MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT:
        {
            // Disconnect client
            disconnectClient(serpHeader.src);
            break;
        }
        case MESSAGE_OPTION_ADVERTISEMENT::OK:
        {
            // Send error message
            uint16_t len = setErrorMessage(serpHeader.src, MESSAGE_OPTION_ERROR::BAD_OPTION);
            sendMessage(serpHeader.src, len);
            break;
        }
    }
}

/// Handles an echo message that was sent addressed to this server
static void handleEchoMessage(const SERP_Header& serpHeader, MESSAGE_OPTION_ECHO messageOption, uint16_t messageLen)
{
    switch (messageOption) {
        case MESSAGE_OPTION_ECHO::ECHO_REPLY:
        {
            break;
        }
        case MESSAGE_OPTION_ECHO::ECHO_REQUEST:
        {
            setSERPHeader(SERP_Header(0, serpHeader.src, messageLen, 0, 1, getNextMessageID()));
            setSMPHeader(SMP_Header(MESSAGE_TYPE::ECHO, MESSAGE_OPTION_ECHO::ECHO_REPLY));
            sendMessage(lastRemoteAddr, messageLen);
            break;
        }
    }
}

/// Sends a ping request to the given client
static void sendPingRequest(uint16_t dst)
{
    setSERPHeader(SERP_Header(0, dst, sizeof(SERP_Header) + sizeof(SMP_Header), 0, 1, getNextMessageID()));
    setSMPHeader(SMP_Header(MESSAGE_TYPE::ECHO, MESSAGE_OPTION_ECHO::ECHO_REQUEST));
    sendMessage(dst, sizeof(SERP_Header) + sizeof(SMP_Header));
}

/// Sends a disconnect message to the given client
static void sendDisconnectMessage(uint16_t dst)
{
    setSERPHeader(SERP_Header(0, dst, sizeof(SERP_Header) + sizeof(SMP_Header), 0, 1, getNextMessageID()));
    setSMPHeader(SMP_Header(MESSAGE_TYPE::ADVERTISEMENT, MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT));
    sendMessage(dst, sizeof(SERP_Header) + sizeof(SMP_Header));
}

void relayMessageSingleCast(const SERP_Header& serpHeader)
{
    sendMessage(serpHeader.dst, serpHeader.len);
}

void relayMessageMultiCast(const SERP_Header& serpHeader, int messageLen)
{
    if (messageLen <= serpHeader.len) {
        log("[ERROR]: Tried to multicast message with no destination addresses!");
    }
    else {
        SERP_Header sendSerpHeader = serpHeader;
        for (unsigned int i = 0; i < (messageLen - serpHeader.len) / 2; ++i)
        {
            uint16_t dst = ntohs(*reinterpret_cast<uint16_t*>(buffer.data() + serpHeader.len + i*2));
            sendSerpHeader.dst = dst;
            setSERPHeader(sendSerpHeader);
            sendMessage(dst, serpHeader.len);
        }
    }
}

void cleanup()
{
    running = false;
    sleeping = false;
    if (logger && logger->is_open()) logger->close();
    // Send disconnect messages to all connected clients
    SERP_Header disconnectSERPHeader(0, 0, sizeof(SERP_Header) + sizeof(SMP_Header), 0, 1, getNextMessageID());
    setSMPHeader(SMP_Header(MESSAGE_TYPE::ADVERTISEMENT, MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT));
    for (const auto& client : clients)
    {
        disconnectSERPHeader.dst = client.first;
        setSERPHeader(disconnectSERPHeader);
        sendMessage(client.first, disconnectSERPHeader.len);
    }
    // Close server
    close(serverSocket);
}

bool initialize()
{
    running = true;
    sleeping = false;
    logger = std::make_unique<std::ofstream>("SERP_log.txt");
    // Create socket
    auto sock = createUDPSocket();
    if (!sock.has_value()) {
        log("[ERROR]: creating UDP socket failed.");
        cleanup();
        return false;
    }
    int sockValue = sock.value();
    serverSocket = sockValue;
    // Server address
    auto addr_server = createSockAddress(SERP_SERVER_PORT);
    if (!addr_server.has_value()) {
        log("[ERROR]: creating server socket address failed.");
        cleanup();
        return false;
    }
    // Bind socket to address
    if (!bindUDPSocket(sock.value(), reinterpret_cast<sockaddr*>(&addr_server.value()), sizeof(sockaddr_in)))
    {
        log("[ERROR]: Binding UDP socket to socket address failed with error code", errno);
        cleanup();
        return false;
    }
    // Set socket timeout
    setSocketTimeoutMicroseconds(sock.value(), SOCKET_TIMEOUT_SECONDS, 0);
    // Set message id
    nextMessageID = 1;
    // Seed rng
    std::random_device rd;
    rng = std::make_unique<std::default_random_engine>(rd());
    return true;
}

void startMainLoop()
{
    log("starting SERP daemon. Current time:");
    log(getCurrentTime().c_str());
    auto lastClientUpdate = std::chrono::high_resolution_clock::now();
    double serverTimeout = 0.0;
    while (running) {
        auto result = receiveMessage();
        if (result.has_value())
        {
            // Wake up if we were sleeping!
            if (sleeping) {
                sleeping = false;
                log("Received message, waking up!");
            }
            SERP_Header& serpHeader = result.value().first;
            uint16_t& messageLen = result.value().second;
            if (messageLen < serpHeader.len) {
                log("[ERROR]: Received message where the real messageLength is smaller than the length specified in the header.");
                continue;
            }
            if (serpHeader.dst == 0) {
                // The message is addressed at the server!
                if (messageLen < sizeof(SERP_Header) + sizeof(SMP_Header)) {
                    log("[ERROR]: Received message addressed to the server with no SMP header.");
                    continue;
                }
                if (messageLen > MAX_MESSAGE_LEN) {
                    log("[ERROR]: Received message that was larger than the maximum allowed message size.");
                    continue;
                }
                SMP_Header smpHeader = extractSMPHeader();
                switch (static_cast<MESSAGE_TYPE>(smpHeader.type)) {
                    case MESSAGE_TYPE::ADVERTISEMENT:
                    {
                        handleAdvertisement(serpHeader, static_cast<MESSAGE_OPTION_ADVERTISEMENT>(smpHeader.options));
                        break;
                    }
                    case MESSAGE_TYPE::ERROR:
                    {
                        if (messageLen > sizeof(SERP_Header) + sizeof(SMP_Header))
                            log("[ERROR] client ", serpHeader.src, "sent error code ", smpHeader.options, " with message \"", extractDataAsString(messageLen), "\"");
                        else
                            log("[ERROR] client ", serpHeader.src, "sent error code ", smpHeader.options);
                        break;
                    }
                    case MESSAGE_TYPE::ECHO:
                    {
                        handleEchoMessage(serpHeader, static_cast<MESSAGE_OPTION_ECHO>(smpHeader.options), messageLen);
                        break;
                    }
                }
            }
            else {
                // The message is addressed to a client!
                if (serpHeader.dst == SERP_DST_MULTICAST) {
                    relayMessageMultiCast(serpHeader, messageLen);
                }
                else {
                    relayMessageSingleCast(serpHeader);
                }
            }
            // In both cases: Reset timeout of client who sent the message
            auto it = clients.find(serpHeader.src);
            if (it != clients.end()) it->second.timeout = 0;
            // Reset timeout of server
            serverTimeout = 0.0;
        }
        // Check if we need to update client list
        if (sleeping) {
            sleep(SLEEP_DURATION);
        }
        else {
            auto time = std::chrono::high_resolution_clock::now();
            unsigned duration = std::chrono::duration_cast<std::chrono::seconds>(time - lastClientUpdate).count();
            if (duration > PROCESS_CLIENT_TIMEOUT_EVERY_SECONDS) {
                if (clients.empty()) {
                    serverTimeout += duration;
                    if (serverTimeout > SLEEP_TIMEOUT) {
                        sleeping = true;
                        log("Going to sleep ...");
                    }
                }
                else {
                    // Process client timeouts
                    std::vector<uint16_t> disconnectIDs;
                    for (auto& client : clients) {
                        client.second.timeout += PROCESS_CLIENT_TIMEOUT_EVERY_SECONDS;
                        if (client.second.timeout > CLIENT_TIMEOUT_SECONDS) {
                            log("Disconnecting client with id ", client.second.id, " because of timeout:", client.second.timeout, " seconds.");
                            sendDisconnectMessage(client.first);
                            disconnectIDs.push_back(client.first);
                        }
                        else if (client.second.timeout > SEND_PING_TO_CLIENT_AFTER_SECONDS) {
                            sendPingRequest(client.first);
                        }
                    }
                    for (const auto& clientID : disconnectIDs) {
                        disconnectClient(clientID);
                    }
                }
            }
            lastClientUpdate = time;
        }
    }
    log("shutting down SERP daemon. Current time:");
    log(getCurrentTime().c_str());
    // cleanup
    cleanup();
}