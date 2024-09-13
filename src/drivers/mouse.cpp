#include <drivers/mouse.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char* str);

MouseEventHandler::MouseEventHandler()
{

}

void MouseEventHandler::OnActivate()
{

}

void MouseEventHandler::OnMouseDown(uint8_t button)
{

}

void MouseEventHandler::OnMouseUp(uint8_t button)
{

}

void MouseEventHandler::OnMouseMove(int x, int y)
{

}

MouseDriver::MouseDriver(InterruptManager* interruptManager, MouseEventHandler* mhandler)
: InterruptHandler(0x2C, interruptManager),
dataPort(0x60),
commandPort(0x64)
{
    this->mhandler = mhandler;
}

MouseDriver::~MouseDriver()
{

}

void MouseDriver::Activate()
{
    offset = 0;
    buttons = 0;

    commandPort.Write(0xA8);    // Activate Interrupts 
    commandPort.Write(0x20);    // Get current state

    uint8_t status = dataPort.Read() | 2;

    commandPort.Write(0x60);    // Set state
    dataPort.Write(status);

    commandPort.Write(0xD4);
    dataPort.Write(0xF4);

    dataPort.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandPort.Read();
    if (!(status & 0x20))
        return esp;

    buffer[offset] = dataPort.Read();

    if (mhandler == 0)
        return esp;

    offset = (offset + 1) % 3;
    static uint16_t* VideoMemory = (uint16_t*)0xB8000;

    if (offset == 0)
    {
        if (buffer[1] != 0 || buffer[2] != 0)
        {
            mhandler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        }

        for (uint8_t i = 0; i < 3; i++)
        {
            if ((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
            {
                if (buttons & (0x1<<i))
                    mhandler->OnMouseUp(i+1);
                else
                    mhandler->OnMouseDown(i+1);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}
