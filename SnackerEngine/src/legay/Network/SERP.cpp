#include "Network/SERP.h"

#include <winsock.h>

namespace SnackerEngine
{

	void SERP_Header::turnToHostByteOrder()
	{
		src = ntohs(src);
		dst = ntohs(dst);
		len = ntohs(len);
		id = ntohl(id);
		flags = ntohl(flags);
	}

	void SERP_Header::turnToNetworkByteOrder()
	{
		src = htons(src);
		dst = htons(dst);
		len = htons(len);
		id = htonl(id);
		flags = htonl(flags);
	}

	void SERP_Header::setFlag(FlagType flagType)
	{
		flags |= (0b1 << static_cast<unsigned int>(flagType));
	}

	bool SERP_Header::getFlag(FlagType flagType) const
	{
		return SnackerEngine::getFlag(flags, flagType);
	}

	bool getFlag(uint32_t flags, SERP_Header::FlagType flagType)
	{
		return flags & (0b1 << static_cast<unsigned int>(flagType));
	}

}