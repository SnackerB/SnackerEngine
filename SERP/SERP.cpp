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