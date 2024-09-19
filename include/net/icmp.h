#ifndef __MYOS__NET__ICMP__H
#define __MYOS__NET__ICMP__H

#include <common/types.h>
#include <net/ipv4.h>

namespace myos
{
    namespace net
    {
        struct InternetControlMessageProtocolMessage 
        {
            myos::common::uint8_t type;
            myos::common::uint8_t code;

            myos::common::uint16_t checksum;
            myos::common::uint32_t data;
        } __attribute__((packed));

        class InternetControlMessageProtocol : InternetProtocolHandler
        {
            public:
                InternetControlMessageProtocol(InternetProtocolProvider* backend);
                ~InternetControlMessageProtocol();

                bool OnInternetProtocolReceived(myos::common::uint32_t srcIP_BE, myos::common::uint32_t dstIP_BE, myos::common::uint8_t* internetProtocolPayload, myos::common::uint32_t size);
                void RequestEchoReply(myos::common::uint32_t ip_be);
        };
    }
}

#endif