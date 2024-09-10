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

    static bool shift = false;

    switch (key)
    {
        case 0xFA: break;

        case 0x2: if (shift) printf("!"); else printf("1"); break;
        case 0x3: if (shift) printf("@"); else printf("2"); break;
        case 0x4: if (shift) printf("#"); else printf("3"); break;
        case 0x5: if (shift) printf("$"); else printf("4"); break;
        case 0x6: if (shift) printf("%"); else printf("5"); break;
        case 0x7: if (shift) printf("^"); else printf("6"); break;
        case 0x8: if (shift) printf("&"); else printf("7"); break;
        case 0x9: if (shift) printf("*"); else printf("8"); break;
        case 0xA: if (shift) printf("("); else printf("9"); break;
        case 0xB: if (shift) printf(")"); else printf("0"); break;

        case 0x10: if (shift) printf("Q"); else printf("q"); break;
        case 0x11: if (shift) printf("W"); else printf("w"); break;
        case 0x12: if (shift) printf("E"); else printf("e"); break;
        case 0x13: if (shift) printf("R"); else printf("r"); break;
        case 0x14: if (shift) printf("T"); else printf("t"); break;
        case 0x15: if (shift) printf("Y"); else printf("y"); break;
        case 0x16: if (shift) printf("U"); else printf("u"); break;
        case 0x17: if (shift) printf("I"); else printf("i"); break;
        case 0x18: if (shift) printf("O"); else printf("o"); break;
        case 0x19: if (shift) printf("P"); else printf("p"); break;
        case 0x1A: if (shift) printf("{"); else printf("["); break;
        case 0x1B: if (shift) printf("}"); else printf("]"); break;

        case 0x1E: if (shift) printf("A"); else printf("a"); break;
        case 0x1F: if (shift) printf("S"); else printf("s"); break;
        case 0x20: if (shift) printf("D"); else printf("d"); break;
        case 0x21: if (shift) printf("F"); else printf("f"); break;
        case 0x22: if (shift) printf("G"); else printf("g"); break;
        case 0x23: if (shift) printf("H"); else printf("h"); break;
        case 0x24: if (shift) printf("J"); else printf("j"); break;
        case 0x25: if (shift) printf("K"); else printf("k"); break;
        case 0x26: if (shift) printf("L"); else printf("l"); break;
        case 0x27: if (shift) printf(":"); else printf(";"); break;
        case 0x28: if (shift) printf("\""); else printf("'"); break;

        case 0x2C: if (shift) printf("Z"); else printf("z"); break;
        case 0x2D: if (shift) printf("X"); else printf("x"); break;
        case 0x2E: if (shift) printf("C"); else printf("c"); break;
        case 0x2F: if (shift) printf("V"); else printf("v"); break;
        case 0x30: if (shift) printf("B"); else printf("b"); break;
        case 0x31: if (shift) printf("N"); else printf("n"); break;
        case 0x32: if (shift) printf("M"); else printf("m"); break;
        case 0x33: if (shift) printf("<"); else printf(","); break;
        case 0x34: if (shift) printf(">"); else printf("."); break;
        case 0x35: if (shift) printf("?"); else printf("/"); break;

        case 0x1C: printf("\n"); break;
        case 0x39: printf(" "); break;

        case 0x2A: case 0x36: shift = true; break;
        case 0xAA: case 0xB6: shift = false; break;

        case 0x45: case 0xC5: break;
        default:
            if (key < 0x80)
            {
                char* foo = "KEYBOARD 0x00";
                char* hex = "0123456789ABCDEF";
                foo[11] = hex[(key >> 4) & 0x0F];
                foo[12] = hex[key & 0x0F];
                printf(foo);
                break;
            }
    }

    return esp;
}
