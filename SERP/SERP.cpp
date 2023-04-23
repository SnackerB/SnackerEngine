#include "SERP.h"

#include <arpa/inet.h>

void SERP_Header::turnToHostByteOrder()
{
    src = ntohs(src);
    dst = ntohs(dst);
    len = ntohs(len);
    id = ntohl(id);
}

void SERP_Header::turnToNetworkByteOrder()
{
    src = htons(src);
    dst = htons(dst);
    len = htons(len);
    id = htonl(id);
}

void SERP_Header::setFlag(FlagType flagType)
{
	flags |= (0b1 << static_cast<unsigned int>(flagType));
}

bool SERP_Header::getFlag(FlagType flagType) const
{
    return flags & (0b1 << static_cast<unsigned int>(flagType));
}