#include "Network/SMP.h"

#include <winsock.h>

namespace SnackerEngine
{

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

}