#include <net/tcp.h>

using namespace myos;
using namespace myos::common;
using namespace myos::net;

void printf(char*);
void printfHex(uint8_t key);

TransmissionControlProtocolHandler::TransmissionControlProtocolHandler()
{

}

TransmissionControlProtocolHandler::~TransmissionControlProtocolHandler()
{

}

void TransmissionControlProtocolHandler::HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, uint8_t* data, uint16_t size)
{

}


TransmissionControlProtocolSocket::TransmissionControlProtocolSocket(TransmissionControlProtocolProvider* backend)
{
    this->backend = backend;
    handler = 0;
    state = CLOSED;
}

TransmissionControlProtocolSocket::~TransmissionControlProtocolSocket()
{

}

void TransmissionControlProtocolSocket::HandleTransmissionControlProtocolMessage(uint8_t* data, uint16_t size)
{
    if (handler != 0)
        handler->HandleTransmissionControlProtocolMessage(this, data, size);
}

void TransmissionControlProtocolSocket::Send(uint8_t* data, uint16_t size)
{
    backend->Send(this, data, size);
}

void TransmissionControlProtocolSocket::Disconnect()
{
    backend->Disconnect(this);
}

TransmissionControlProtocolProvider::TransmissionControlProtocolProvider(InternetProtocolProvider* backend) :
InternetProtocolHandler(backend, 0x06)
{
    for (int i = 0; i < 65535; i++)
        sockets[i] = 0;
    numSockets = 0;
    freePort = 1024;
}

TransmissionControlProtocolProvider::~TransmissionControlProtocolProvider()
{

}







bool TransmissionControlProtocolProvider::OnInternetProtocolReceived(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size)
{
    if (size < sizeof(TransmissionControlProtocolHeader))
        return false;


    // for (uint16_t i = 0; i < numSockets && sockets == 0; i++)
    // {
    //     if (sockets[i] == socket)
    //     {
    //         sockets[i] = sockets[--numSockets];
    //         MemoryManager::activeMemoryManager->free(socket);
    //         break;
    //     }
    // }



    TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)internetProtocolPayload;

    uint16_t localPort = msg->dstPort;
    uint16_t remotePort = msg->srcPort;

    TransmissionControlProtocolSocket* socket = 0;
    for (uint16_t i = 0; i < numSockets && sockets == 0; i++)
    {
        if (sockets[i]->localPort == msg->dstPort 
        && sockets[i]->localIP == dstIP_BE
        && sockets[i]->state == LISTEN)
        {
            socket = sockets[i];
            socket->state = SYN_RECEIVED;
            socket->remotePort = msg->srcPort;
            socket->remoteIP = srcIP_BE;
        }
        else if (sockets[i]->localPort == msg->dstPort 
        && sockets[i]->localIP == dstIP_BE 
        && sockets[i]->remotePort == msg->srcPort 
        && sockets[i]->remoteIP == srcIP_BE)
        {
            socket = sockets[i];
        }
    }

    if (socket != 0)
        socket->HandleTransmissionControlProtocolMessage(internetProtocolPayload + sizeof(TransmissionControlProtocolHeader), size - sizeof(TransmissionControlProtocolHeader));

    return false;
}






// ---------------------------------------------------------------------------------------------------

uint32_t bigEndian32(uint32_t x)
{
    return  (x & 0xFF000000) >> 24
        |   (x & 0x00FF0000) >> 8
        |   (x & 0x0000FF00) << 8
        |   (x & 0x000000FF) << 24;
}

void TransmissionControlProtocolProvider::Send(TransmissionControlProtocolSocket* socket, uint8_t* data, uint16_t size, uint16_t flags)
{
    uint32_t totalLength = size + sizeof(TransmissionControlProtocolHeader);
    uint16_t lengthInclPHdr = totalLength + sizeof(TransmissionControlProtocolPsuedoHeader);
    uint8_t* buffer = (uint8_t*)MemoryManager::activeMemoryManager->malloc(lengthInclPHdr);

    TransmissionControlProtocolPsuedoHeader* phdr = (TransmissionControlProtocolPsuedoHeader*)buffer;
    TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)(buffer + sizeof(TransmissionControlProtocolPsuedoHeader));
    uint8_t* buffer2 = buffer + sizeof(TransmissionControlProtocolHeader) + sizeof(TransmissionControlProtocolPsuedoHeader);

    msg->headerSize32 = sizeof(TransmissionControlProtocolHeader) / 4;
    msg->srcPort = socket->localPort;
    msg->dstPort = socket->remotePort;
    msg->acknowledgementNumber = bigEndian32( socket->acknowledgementNumber );
    msg->sequenceNumber = bigEndian32( socket->sequenceNumber );
    msg->reserved = 0;
    msg->flags = flags;
    msg->windowSize = 0xFFFF;
    msg->urgentPtr = 0;

    msg->options = ((flags & SYN) != 0) ? 0xB4050402 : 0;

    socket->sequenceNumber += size;

    for (int i = 0; i < size; i++) 
        buffer2[i] = data[i];
    
    phdr->srcIP = socket->localIP;
    phdr->dstIP = socket->remoteIP;
    phdr->protocol = 0x0600;
    phdr->totalLength = ((totalLength & 0x00FF) << 8) | ((totalLength & 0xFF00) >> 8);

    msg->checksum = 0;
    msg->checksum = InternetProtocolProvider::Checksum((uint16_t*)buffer, lengthInclPHdr);

    InternetProtocolHandler::Send(socket->remoteIP, (uint8_t*)msg, totalLength);
    MemoryManager::activeMemoryManager->free(buffer);
}

TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Connect(uint32_t ip, uint16_t port)
{
    TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(TransmissionControlProtocolSocket));

    if (socket != 0)
    {
        new (socket) TransmissionControlProtocolSocket(this);

        socket->remotePort = port;
        socket->remoteIP = ip;
        socket->localPort = freePort++;
        socket->localIP = backend->GetIPAddress();

        socket->remotePort = ((socket->remotePort & 0xFF00) >> 8) | ((socket->remotePort & 0x00FF) << 8);
        socket->localPort = ((socket->localPort & 0xFF00) >> 8) | ((socket->localPort & 0x00FF) << 8);

        sockets[numSockets++] = socket;
        socket->state = SYN_SENT;

        socket->sequenceNumber = 0xbeefcafe;

        Send(socket, 0, 0, SYN);
    }

    return socket;
}

void TransmissionControlProtocolProvider::Disconnect(TransmissionControlProtocolSocket* socket)
{
    socket->state = FIN_WAIT1;
    Send(socket, 0, 0, FIN + ACK);
    socket->sequenceNumber++;
}

TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Listen(uint16_t port)
{
    TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(TransmissionControlProtocolSocket));

    if (socket != 0)
    {
        new (socket) TransmissionControlProtocolSocket(this);

        socket->state = LISTEN;
        socket->localIP = backend->GetIPAddress();
        socket->localPort = ((port & 0xFF00) >> 8) | ((port & 0x00FF) << 8);

        sockets[numSockets++] = socket;
    }

    return socket;
}

void TransmissionControlProtocolProvider::Bind(TransmissionControlProtocolSocket* socket, TransmissionControlProtocolHandler* handler)
{
    socket->handler = handler;
}
