#ifndef __MYOS__NET__TCP_H
#define __MYOS__NET__TCP_H

#include <common/types.h>
#include <net/ipv4.h>
#include <memorymanagement.h>

namespace myos
{
    namespace net
    {
        enum TransmissionControlProtocolSocketState
        {
            CLOSED,
            LISTEN,
            SYN_SENT,
            SYN_RECEIVED,

            ESTABLISHED,

            FIN_WAIT1,
            FIN_WAIT2,
            CLOSING,
            TIME_WAIT,

            CLOSE_WAIT
            // LAST_ACK,
        };

        enum TransmissionControlProtocolFlag
        {
            FIN = 1,
            SYN = 2,
            RST = 4,
            PSH = 8,
            ACK = 16,
            URG = 32,
            ECE = 64,
            CWR = 128,
            NS = 256
        };

        struct TransmissionControlProtocolHeader
        {
            myos::common::uint16_t srcPort;
            myos::common::uint16_t dstPort;
            myos::common::uint32_t sequenceNumber;
            myos::common::uint32_t acknowledgementNumber;

            myos::common::uint8_t reserved : 4;
            myos::common::uint8_t headerSize32 : 4;
            myos::common::uint8_t flags;


            myos::common::uint16_t windowSize;
            myos::common::uint16_t checksum;
            myos::common::uint16_t urgentPtr;

            myos::common::uint32_t options;
        } __attribute__((packed));

        struct TransmissionControlProtocolPsuedoHeader
        {
            myos::common::uint32_t srcIP;
            myos::common::uint32_t dstIP;
            myos::common::uint16_t protocol;
            myos::common::uint16_t totalLength;
        } __attribute__((packed));
        
        class TransmissionControlProtocolSocket;
        class TransmissionControlProtocolProvider;

        class TransmissionControlProtocolHandler
        {
            public:
                TransmissionControlProtocolHandler();
                ~TransmissionControlProtocolHandler();
                virtual bool HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, myos::common::uint8_t* data, myos::common::uint16_t size);
        };

        class TransmissionControlProtocolSocket
        {
            friend class TransmissionControlProtocolProvider;
            protected:
                myos::common::uint16_t remotePort;
                myos::common::uint32_t remoteIP;
                myos::common::uint16_t localPort;
                myos::common::uint32_t localIP;
                myos::common::uint32_t sequenceNumber;
                myos::common::uint32_t acknowledgementNumber;
                TransmissionControlProtocolProvider* backend;
                TransmissionControlProtocolHandler* handler;

                TransmissionControlProtocolSocketState state;
            public:
                TransmissionControlProtocolSocket(TransmissionControlProtocolProvider* backend);
                ~TransmissionControlProtocolSocket();                
                virtual bool HandleTransmissionControlProtocolMessage(myos::common::uint8_t* data, myos::common::uint16_t size);
                virtual void Send(myos::common::uint8_t* data, myos::common::uint16_t size);
                virtual void Disconnect();
        };

        class TransmissionControlProtocolProvider : InternetProtocolHandler
        {
            protected:
                TransmissionControlProtocolSocket* sockets[65535];
                myos::common::uint16_t numSockets;
                myos::common::uint16_t freePort;
            public:
                TransmissionControlProtocolProvider(InternetProtocolProvider* backend);
                ~TransmissionControlProtocolProvider();
                
                virtual bool OnInternetProtocolReceived(myos::common::uint32_t srcIP_BE, myos::common::uint32_t dstIP_BE, myos::common::uint8_t* internetProtocolPayload, myos::common::uint32_t size);

                virtual TransmissionControlProtocolSocket* Connect(myos::common::uint32_t ip, myos::common::uint16_t port);
                virtual void Disconnect(TransmissionControlProtocolSocket* socket);
                virtual void Send(TransmissionControlProtocolSocket* socket, myos::common::uint8_t* data, myos::common::uint16_t size, myos::common::uint16_t flags = 0);

                virtual TransmissionControlProtocolSocket* Listen(myos::common::uint16_t port);
                virtual void Bind(TransmissionControlProtocolSocket* socket, TransmissionControlProtocolHandler* handler);
        };
    }
}

#endif