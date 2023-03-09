#include "network.h"
#include <cstring>

std::optional<int> createUDPSocket()
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) return {};
    return sock;
}

void setSocketTimeoutMicroseconds(int sock, int seconds, int microseconds)
{
    struct timeval read_timeout{};
    read_timeout.tv_sec = seconds;
    read_timeout.tv_usec = microseconds;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));
}

std::optional<sockaddr_in> createSockAddress(const int& port, const char* ip)
{
    sockaddr_in addr{};
    // First set everything to zero
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip, (void*)&addr.sin_addr.s_addr);
    if (ret != 1) return {};
    return addr;
}

bool bindUDPSocket(int sock, sockaddr* addr, int addr_len)
{
    int ret = bind(sock, addr, addr_len);
    if (ret == -1) return false;
    return true;
}

std::optional<int> receiveMessage(const int& sock, sockaddr& remoteAddr, unsigned int& remoteAddrLen, void* buffer,  const int& buffer_len)
{
    auto ret = recvfrom(sock, buffer, buffer_len, 0, &remoteAddr, &remoteAddrLen);
    if (ret == -1)
    {
        if (errno == 11) // Timeout
            return 0;
        return {};
    }
    return ret;
}

std::optional<int> sendMessage(const int& sock, const sockaddr_in& remoteAddr, void* buffer, const int& len)
{
    auto ret = sendto(sock, buffer, len, 0, reinterpret_cast<const sockaddr*>(&remoteAddr), sizeof(sockaddr_in));
    if (ret == -1) return {};
    return ret;
}
