#include "Network.h"

#include <exception>
#include <random>

#ifdef _WINDOWS
#include <WinSock2.h>
#endif // _WINDOWS

namespace SnackerEngine
{

	void initializeNetwork()
	{
#ifdef _WINDOWS
		WSAData wsaData;
		WORD DllVersion = MAKEWORD(2, 1);
		if (WSAStartup(DllVersion, &wsaData) != 0) {
			throw std::exception("Winsock Connection Failed!");
		}
#endif // _WINDOWS
		// Seed rng
		//srand(time(NULL));
	}

	void cleanupNetwork()
	{
#ifdef _WINDOWS
		WSACleanup();
#endif // _WINDOWS
	}

}
