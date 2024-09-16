#include <syscalls.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;

SyscallHandler::SyscallHandler(uint8_t interruptNumber, InterruptManager* interruptManager) :
InterruptHandler(interruptNumber + 0x20, interruptManager)
{

}

SyscallHandler::~SyscallHandler()
{

}

void printf(char* str);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpu = (CPUState*)esp;

    switch (cpu->eax)
    {
        case 4:
            printf((char*)cpu->ebx);
            break;
        default:
            break;
    }

    return esp;
}
