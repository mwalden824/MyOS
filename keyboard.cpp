#include "keyboard.h"

void printf(char* str);

KeyboardDriver::KeyboardDriver(InterruptManager* interruptManager)
: InterruptHandler(0x21, interruptManager),
dataPort(0x60),
commandPort(0x64)
{
    while (commandPort.Read() & 0x1)
        dataPort.Read();
    
    commandPort.Write(0xAE);    // Activate Interrupts 
    commandPort.Write(0x20);    // Get current state

    uint8_t status = (dataPort.Read() | 1) & ~0x10;

    commandPort.Write(0x60);    // Set state
    dataPort.Write(status);

    dataPort.Write(0xF4);
}

KeyboardDriver::~KeyboardDriver()
{

}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataPort.Read();

    char* foo = "KEYBOARD 0x00";
    char* hex = "0123456789ABCDEF";
    foo[11] = hex[(key >> 4) & 0x0F];
    foo[12] = hex[key & 0x0F];
    printf(foo);


    return esp;
}
