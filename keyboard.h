#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"
#include "driver.h"
#include "interrupts.h"
#include "port.h"

class KeyboardEventHandler
{
    public:
        KeyboardEventHandler();

        virtual void OnKeyDown(char);
        virtual void OnKeyUp(char);
};

class KeyboardDriver : public InterruptHandler, public Driver
{
    Port8Bit dataPort;
    Port8Bit commandPort;

    KeyboardEventHandler* handler;

    public:
        KeyboardDriver(InterruptManager* interruptManager, KeyboardEventHandler* handler);
        ~KeyboardDriver();
        virtual uint32_t HandleInterrupt(uint32_t esp);
        virtual void Activate();
};

#endif