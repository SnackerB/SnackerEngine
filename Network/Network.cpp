#include "Network.h"

#include <WinSock2.h>
#include <exception>
#include <random>

namespace SnackerEngine
{

	void initializeNetwork()
	{
		WSAData wsaData;
		WORD DllVersion = MAKEWORD(2, 1);
		if (WSAStartup(DllVersion, &wsaData) != 0) {
			throw std::exception("Winsock Connection Failed!");
		}
		// Seed rng
		//srand(time(NULL));
	}

}
