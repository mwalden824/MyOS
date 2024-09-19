#include <net/icmp.h>

using namespace myos;
using namespace myos::common;
using namespace myos::net;

void printf(char*);
void printfHex(uint8_t key);

InternetControlMessageProtocol::InternetControlMessageProtocol(InternetProtocolProvider* backend) :
InternetProtocolHandler(backend, 0x01)
{
}

InternetControlMessageProtocol::~InternetControlMessageProtocol()
{
}

bool InternetControlMessageProtocol::OnInternetProtocolReceived(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size)
{
    printf("Internet Protocol Received");
    if (size < sizeof(InternetControlMessageProtocolMessage))
        return false;
    
    InternetControlMessageProtocolMessage* msg = (InternetControlMessageProtocolMessage*)internetProtocolPayload;

    switch (msg->type)
    {
        case 0:
            printf("\nPING RESPONSE from "); printfHex(srcIP_BE & 0xFF);
            printf("."); printfHex((srcIP_BE >> 8) & 0xFF);
            printf("."); printfHex((srcIP_BE >> 16) & 0xFF);
            printf("."); printfHex((srcIP_BE >> 24) & 0xFF);
            printf("\n");
            break;
        case 8:
            printf("YOU HAVE BEEN PINGED\n");
            msg->type = 0;
            msg->checksum = 0;
            msg->checksum = InternetProtocolProvider::Checksum((uint16_t*)msg, sizeof(InternetControlMessageProtocolMessage));
            return true;
            break;
    }

    return false;
}

void InternetControlMessageProtocol::RequestEchoReply(uint32_t ip_be)
{
    InternetControlMessageProtocolMessage icmp;
    icmp.type = 8; //ping
    icmp.code = 0;
    icmp.data = 0x3713;
    icmp.checksum = 0;
    icmp.checksum = InternetProtocolProvider::Checksum((uint16_t*)&icmp, sizeof(InternetControlMessageProtocolMessage));

    InternetProtocolHandler::Send(ip_be, (uint8_t*)&icmp, sizeof(InternetControlMessageProtocolMessage));
}
