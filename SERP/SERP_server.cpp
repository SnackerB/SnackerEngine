#include <iostream>
#include <cstdint>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <array>
#include <unordered_map>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#define DEBUG

const int max_clients = 50; // maximum number of clients registered in parallel
const int client_timeout = 5; // client timeout
const int update_every = 10; // update client timeout every 10 seconds
const int start_sending_echo = 3; // when client time goes below this threshhold: send echo request!
const int timeout_seconds = 5; // timeout for recvfrom() in seconds
const int max_message_len = 4000; // max mesage length in bytes

struct SRP_header // Snacker Relay Protocol (SRP)
{
    int16_t src; // source number of this packet.
    int16_t dst; // destination number of this packet. If dst == 0xffff (multicast), all recievers are listed after the message.
    int16_t len; // the length of payload + header in bytes.
    /* DATA */
    /* RECIEVERS OF MULTICAST (optional) */
};

struct SMP_header // Snacker Message Protocol (SMP)
{
    int16_t type;
    int16_t options;
    /* DATA */
};

enum MESSAGE_TYPE : uint16_t
{
    ECHO,                   // Used for pings and alive-checks
    ERROR,                  // Used for sending error messages
    ADVERTISEMENT,          // Used to advertise a client to the SRP server
};

enum MESSAGE_OPTION_ECHO : uint16_t
{
    ECHO_REQUEST,           // Requests an echo reply
    ECHO_REPLY,             // answers to an echo request

    /* DATA: none */
};

enum MESSAGE_OPTION_ERROR : uint16_t
{
    TIMEOUT,                // If some action timeouts
    CLIENT_NOT_FOUND,       // If the client someone wanted to send something to was not found additional data: uint16_t dst (the undefined id).
    BAD_TYPE,               // If an unrecognized type was given
    BAD_OPTION,             // If an unrecognized option was given
    TOO_MANY_CLIENTS,       // Too many clients have already connected

    /* DATA: depends on the option! */
};

enum MESSAGE_OPTION_ADVERTISEMENT : uint16_t
{
    REQUEST,          // advertises a client to the server
    OK,               // server sends the client his new id
    DISCONNECT,       // Client disconnecting from server

    /* DATA: the client id (uint16_t) (optional) */
};

struct client
{
    int timeout;
    uint16_t id;
    sockaddr_in addr;
};

void printClient(client c)
{
    std::cout << "{ id: " << std::dec << c.id << ", timeout: " << std::dec << c.timeout << ", addr: ";
    char temp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(c.addr.sin_addr), temp, INET_ADDRSTRLEN);
    for (unsigned int i = 0; i < INET_ADDRSTRLEN; ++i) std::cout << temp[i];
    std::cout << ":" << c.addr.sin_port << " }";
}

// data structures for holding clients
std::array<client, max_clients> clients;
int num_clients = 0;
std::unordered_map<int16_t, size_t> client_map;
// buffer
char buffer[max_message_len];
// Incoming address
sockaddr remoteAddr;
unsigned int remoteAddrLen = sizeof(sockaddr);
// Set to false when client disconnects
bool client_disconnected;

int createUdpSocket()
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1)
        std::cout << "[ERROR]: socket() failed with error code " << errno << "." << std::endl;
    return sock;
}

sockaddr_in createSockAddress(std::string ip, int port)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in)); // zuerst alles auf 0 setzten
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip.c_str(), (void*)&addr.sin_addr.s_addr);
    if (ret != 1)
        std::cout << "[ERROR]: intet_pton() failed with error code " << errno << "." << std::endl;
    return addr;
}

int bindUDPSocket(int sock, sockaddr* addr_server, int addr_len)
{
    int ret = bind(sock, addr_server, addr_len);
    if (ret == -1)
        std::cout << "[ERROR]: bind() failed with error code " << errno << "." << std::endl;
    return ret;
}

void setSocketTimeoutMicroseconds(int sock, int seconds, int microseconds)
{
	struct timeval read_timeout;
	read_timeout.tv_sec = seconds;
	read_timeout.tv_usec = microseconds;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));
}

uint16_t generateRandomID()
{
    uint16_t r = rand();
    while (client_map.find(r) != client_map.end())
    {
        r = rand();
    }
    return r;
}

int disconnect(uint16_t client)
{
    auto it = client_map.find(client);
    if (it != client_map.end())
    {
        int old_pos = it->second;
        clients[old_pos] = clients[num_clients - 1];
        client_map.erase(it);
        auto it2 = client_map.find(clients[it->second].id);
        if (it2 != client_map.end())
            client_map.erase(it2);
        client_map.insert({ clients[it->second].id, old_pos });
        num_clients--;
        return 0;
    }
    return -1;
}

int recieveMessage(int sock, sockaddr* remoteAddr, unsigned int* remoteAddrLen, int buffer_len)
{
    int ret = recvfrom(sock, buffer, buffer_len, 0, remoteAddr, remoteAddrLen);
    if (ret == -1)
    {
        if (errno == 11) // timeout
            return 0;
        std::cout << "[ERROR]: recvfrom() failed with error code " << errno << "." << std::endl;
    }

    return ret;
}

int sendMessage(int sock, sockaddr_in* Addr, int len)
{
#ifdef DEBUG
    char temp[100];
    inet_ntop(AF_INET, &(Addr->sin_addr), temp, INET_ADDRSTRLEN);
    std::cout << "[DEBUG]: message sent: \" ";
    for (unsigned int i = 0; i < len; ++i) std::cout << std::hex << (int)buffer[i] << " ";
    std::cout << "\" to ";
    for (unsigned int i = 0; i < INET_ADDRSTRLEN; ++i) std::cout << temp[i];
    std::cout << ":" << Addr->sin_port << "." << std::endl;
#endif // DEBUG
    int ret = sendto(sock, buffer, len, 0, (sockaddr*)Addr, sizeof(sockaddr_in));
    if (ret == -1)
        std::cout << "[ERROR]: sendto() failed with error code " << errno << "." << std::endl;
    return ret;
}

int sendError(int sock, sockaddr_in Addr, uint16_t dst, MESSAGE_OPTION_ERROR option, uint16_t data = 0) // data is optional!
{
    ((uint16_t*)buffer)[0] = htons(0);
    ((uint16_t*)buffer)[1] = htons(dst);
    ((uint16_t*)buffer)[3] = htons(MESSAGE_TYPE::ERROR);
    ((uint16_t*)buffer)[4] = htons(option);
    uint16_t len = 10;
    switch (option)
    {
        case MESSAGE_OPTION_ERROR::CLIENT_NOT_FOUND:
        {
            ((uint16_t*)buffer)[5] = htons(data);
            len += 2;
            break;
        }
        default:
        {
            break;
        }
    }
    ((uint16_t*)buffer)[2] = htons(len);
    return sendMessage(sock, &Addr, len);
}

int sendMessageAndOption(int sock, sockaddr_in Addr, uint16_t dst, MESSAGE_TYPE type, uint16_t option)
{
    ((uint16_t*)buffer)[0] = htons(0);
    ((uint16_t*)buffer)[1] = htons(dst);
    ((uint16_t*)buffer)[2] = htons(10);
    ((uint16_t*)buffer)[3] = htons(type);
    ((uint16_t*)buffer)[4] = htons(option);
    return sendMessage(sock, &Addr, 10);
}

int sendMessageAndOption(int sock, sockaddr_in Addr, uint16_t dst, MESSAGE_TYPE type, uint16_t option, uint16_t data)
{
    ((uint16_t*)buffer)[0] = htons(0);
    ((uint16_t*)buffer)[1] = htons(dst);
    ((uint16_t*)buffer)[2] = htons(12);
    ((uint16_t*)buffer)[3] = htons(type);
    ((uint16_t*)buffer)[4] = htons(option);
    ((uint16_t*)buffer)[5] = htons(data);
    return sendMessage(sock, &Addr, 12);
}

int relayMessage(int sock, int len, uint16_t src, uint16_t dst)
{
    auto it = client_map.find(dst);
    if (it == client_map.end())
    {
        // client with this number not registered. Send error message.
        sendError(sock, *((sockaddr_in*)&remoteAddr), src, MESSAGE_OPTION_ERROR::CLIENT_NOT_FOUND, dst);
        return 0;
    }
    return sendMessage(sock, &(clients[it->second].addr), len);
}

int relayMessageMulticast(int sock, int startOfAddr, int len, uint16_t src)
{
    int messageLen = startOfAddr;
    while (startOfAddr < len)
    {
        uint16_t dst = *((uint16_t*)(&(buffer[startOfAddr])));
        ((uint16_t*)buffer)[1] = htons(dst);
        relayMessage(sock, messageLen, src, dst);
        startOfAddr += 2;
    }
    return 0;
}

int handlePacketEcho(int sock, uint16_t src, uint16_t len, int ret, uint16_t option)
{
    switch (option) {
        case MESSAGE_OPTION_ECHO::ECHO_REQUEST:
        {
            // send ECHO_REPLY
            sendMessageAndOption(sock, *((sockaddr_in*)&remoteAddr), src, MESSAGE_TYPE::ECHO, MESSAGE_OPTION_ECHO::ECHO_REPLY);
            break;
        }
        case MESSAGE_OPTION_ECHO::ECHO_REPLY:
        {
            // do nothing
            break;
        }
        default:
        {
            // send error to client
            sendError(sock, *((sockaddr_in*)&remoteAddr), src, MESSAGE_OPTION_ERROR::BAD_OPTION);
            break;
        }
    }
    return 0;
}

int handlePacketError(int sock, uint16_t src, uint16_t len, int ret, uint16_t option)
{
    // Display Error on console
    std::cout << "[ERROR from " << src << "]: ";
    switch (option)
    {
        case MESSAGE_OPTION_ERROR::TIMEOUT:
        {
            std::cout << "timeout." << std::endl;
            break;
        }
        case MESSAGE_OPTION_ERROR::CLIENT_NOT_FOUND:
        {
            std::cout << "client " << ntohs(((uint16_t*)buffer)[5]) << " not found." << std::endl;
            break;
        }
        default:
        {
            std::cout << "undefinded error code " << option << "." << std::endl;
        }
    }
    return 0;
}

int handlePacketAdvertisement(int sock, uint16_t src, uint16_t len, int ret, uint16_t option)
{
    switch(option)
    {
        case MESSAGE_OPTION_ADVERTISEMENT::REQUEST:
        {
            if (num_clients >= max_clients)
            {
                // send error to client if too many cients are already connected
                sendError(sock, *((sockaddr_in*)&remoteAddr), src, MESSAGE_OPTION_ERROR::TOO_MANY_CLIENTS);
                break;
            }
            uint16_t new_id = generateRandomID();
            clients[num_clients].id = new_id;
            clients[num_clients].timeout = client_timeout;
            clients[num_clients].addr = *((sockaddr_in*)(&remoteAddr));

#ifdef DEBUG
            char temp[100];
            inet_ntop(AF_INET, &(clients[num_clients].addr.sin_addr), temp, INET_ADDRSTRLEN);
            std::cout << "[DEBUG] saved new client: "; printClient(clients[num_clients]); std::cout << std::endl;
#endif // DEBUG
            client_map.insert({new_id, num_clients});
            num_clients++;

            // Send advertisement OK
            sendMessageAndOption(sock, clients[num_clients-1].addr, src, MESSAGE_TYPE::ADVERTISEMENT, MESSAGE_OPTION_ADVERTISEMENT::OK, new_id);
            break;
        }
        case MESSAGE_OPTION_ADVERTISEMENT::OK:
        {
            // send error to client
            sendError(sock, *((sockaddr_in*)&remoteAddr), src, MESSAGE_OPTION_ERROR::BAD_OPTION);
            break;
        }
        case MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT:
        {
            disconnect(src);
            break;
        }
        default:
        {
            // send error to client
            sendError(sock, *((sockaddr_in*)&remoteAddr), src, MESSAGE_OPTION_ERROR::BAD_OPTION);
            break;
        }
    }
    return 0;
}

int handlePacket(int sock, uint16_t src, uint16_t len, int ret)
{
    if (ret >= 10)
    {
        uint16_t type = ntohs(((uint16_t*)buffer)[3]);
        uint16_t option = ntohs(((uint16_t*)buffer)[4]);
        switch (type)
        {
            case MESSAGE_TYPE::ECHO:
            {
                return handlePacketEcho(sock, src, len, ret, option);
                break;
            }
            case MESSAGE_TYPE::ERROR:
            {
                return handlePacketError(sock, src, len, ret, option);
                break;
            }
            case MESSAGE_TYPE::ADVERTISEMENT:
            {
                return handlePacketAdvertisement(sock, src, len, ret, option);
                break;
            }
            default:
            {
                // send error to client
                sendError(sock, *((sockaddr_in*)&remoteAddr), src, MESSAGE_OPTION_ERROR::BAD_TYPE);
                break;
            }
        }
    }
    return 0;
}

int resetTimeout(uint16_t src)
{
    auto it = client_map.find(src);
    if (it != client_map.end())
    {
        clients[it->second].timeout = client_timeout;
        return 0;
    }
    return -1;
}

int main()
{
    // Create socket
    int sock = createUdpSocket();
    if (sock == -1) return -1;

    // Server address
    sockaddr_in addr_server = createSockAddress("0.0.0.0", 33333); // IP-Addr: INADDR_ANY

    // Bind socket to address
    if (bindUDPSocket(sock, (sockaddr*)(&addr_server), sizeof(addr_server)) == -1)
    {
        close(sock);
        return -1;
    }

    // Set socket timeout
    setSocketTimeoutMicroseconds(sock, timeout_seconds, 0);

    // intialize random seed
    srand(time(NULL));

    // Main loop
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    bool running = true;
    while (running)
    {
        // ================= STAGE 1: recieve package (or timeout) ===============================
        int ret = recieveMessage(sock, &remoteAddr, &remoteAddrLen, max_message_len);

        if (ret == -1)
            return -1;
        if (ret > 0)
        {
            // A message was recieved
#ifdef DEBUG
            if (ret > 0)
            {
                char temp[100];
                sockaddr_in tempAddr = *((sockaddr_in*)(&remoteAddr));
                inet_ntop(AF_INET, &(tempAddr.sin_addr), temp, INET_ADDRSTRLEN);
                std::cout << "[DEBUG]: message recieved: \" ";
                for (unsigned int i = 0; i < ret; ++i) std::cout << std::hex << (int)buffer[i] << " ";
                std::cout << "\" from ";
                for (unsigned int i = 0; i < INET_ADDRSTRLEN; ++i) std::cout << temp[i];
                std::cout << ":" << tempAddr.sin_port << "." << std::endl;
            }
#endif // DEBUG
            if (ret >= 6)
            {

                uint16_t src = ntohs(((uint16_t*)buffer)[0]);
                uint16_t dst = ntohs(((uint16_t*)buffer)[1]);
                uint16_t len = ntohs(((uint16_t*)buffer)[2]);

                switch (dst) {
                    case 0:
                    {
                        // The packet is targeted at the server
                        handlePacket(sock, src, len, ret);
                        break;
                    }
                    case 0xffff:
                    {
                        // The packet is multicast
                        relayMessageMulticast(sock, len, ret, src);
                        break;
                    }
                    default:
                    {
                        // The packet is targeted at a client
                        relayMessage(sock, ret, src, dst);
                        break;
                    }
                }
                // packet was recieved: update timeout for the sender!
                resetTimeout(src);
            }
        }
        // ================= STAGE 2: update client list ===============================
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
        if (duration >= update_every)
        {
            begin = std::chrono::steady_clock::now(); // reset clock
            for (unsigned int i = 0; i < num_clients; ++i)
            {
                clients[i].timeout--;
                if (clients[i].timeout <= 0)
                {
                    disconnect(clients[i].id);
                    i--;
                }
                else if (clients[i].timeout <= start_sending_echo)
                {
                    sendMessageAndOption(sock, clients[i].addr, clients[i].id, MESSAGE_TYPE::ECHO, MESSAGE_OPTION_ECHO::ECHO_REQUEST);
                }
            }
            std::cout << "[UPDATE]: Currently " << num_clients << " clients connected." << std::endl;
        }
    }

    close(sock);
    return 0;
}
