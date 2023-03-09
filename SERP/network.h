#ifndef SERP_NETWORK_H
#define SERP_NETWORK_H

#include <optional>
#include <arpa/inet.h>

/// Creates a UDP socket. Returns empty optional on failure.
std::optional<int> createUDPSocket();

/// Sets the duration after which a socket times out during a call to rcv().
void setSocketTimeoutMicroseconds(int sock, int seconds, int microseconds);

/// Creates a socket address. Default value for ip: INADDR_ANY. Returns empty optional on failure.
std::optional<sockaddr_in> createSockAddress(const int& port, const char* ip = "0.0.0.0");

/// Binds an UDP socket to a socket address. Returns true on success and false on failure.
bool bindUDPSocket(int sock, sockaddr* addr, int addr_len);

/// Calls recv() and tries to receive a message. Returns the length of the message if a message was received. Returns 0
/// if no message was present (timeout). Returns an empty optional, if an error occurred. If a message was received,
/// the remote address and its length is saved in the respective variables.
std::optional<int> receiveMessage(const int& sock, sockaddr& remoteAddr, unsigned int& remoteAddrLen, void* buffer, const int& buffer_len);

/// Calls send() and tries to send a message. Returns the number of sent bytes if sending was successful, and an
/// empty optional if an error occurred.
std::optional<int> sendMessage(const int& sock, const sockaddr_in& remoteAddr, void* buffer, const int& len);

#endif //SERP_NETWORK_H
