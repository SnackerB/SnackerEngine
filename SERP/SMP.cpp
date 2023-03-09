#include "SMP.h"

#include <arpa/inet.h>

void SMP_Header::turnToHostByteOrder()
{
    type = ntohs(type);
    options = ntohs(options);
}

void SMP_Header::turnToNetworkByteOrder()
{
    type = htons(type);
    options = htons(options);
}