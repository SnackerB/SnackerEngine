#ifndef SERP_SERP_H
#define SERP_SERP_H

#include <cinttypes>

/// Snacker Engine Relay Protocol (SERP)
struct SERP_Header
{
    uint16_t src;    // Source number of this packet.
    uint16_t dst;    // Destination number of this packet. If dst == SERP_DST_MULTICAST, all receivers are listed after the
                    // message.
    uint16_t len;    // The length of payload + header in bytes.
    uint8_t part;    // For large messages: Index of which part of the total message this partial message is
    uint8_t total;   // For large messages: saves how many parts make up this message in total
    uint32_t id;     // unique (random) id of this message. Used to match different parts of a message together.
                    // The ids are only required to be unique for a single sender!
    /* DATA (optional) */
    /* RECEIVERS OF MULTICAST (optional) */
    SERP_Header()
        : src(0), dst(0), len(0), part(0), total(0), id(0) {}
    SERP_Header(const uint16_t& src, const uint16_t& dst, const uint16_t& len, const uint8_t& part, const uint8_t& total,
                const uint32_t& id)
                : src(src), dst(dst), len(len), part(part), total(total), id(id) {}

    void turnToHostByteOrder();
    void turnToNetworkByteOrder();
};

#define SERP_DST_MULTICAST 0xffff

#endif //SERP_SERP_H
