#include <drivers/ata.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char*);
void printfHex(uint8_t key);

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(uint16_t portBase, bool master) :
dataPort(portBase),
errorPort(portBase + 1),
sectorCountPort(portBase + 2),
lbaLowPort(portBase + 3),
lbaMidPort(portBase + 4),
lbaHighPort(portBase + 5),
devicePort(portBase + 6),
commandPort(portBase + 7),
controlPort(portBase + 0x206)
{
    this->master = master;
    bytesPerSector = 512;
}

AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment()
{

}

void AdvancedTechnologyAttachment::Identify()
{
    devicePort.Write(master ? 0xA0 : 0xB0);
    controlPort.Write(0);

    devicePort.Write(0xA0);
    uint8_t status = commandPort.Read();
    if (status == 0xFF)
        return;

    devicePort.Write(master ? 0xA0 : 0xB0);
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHighPort.Write(0);
    commandPort.Write(0xEC);

    status = commandPort.Read();
    if (status == 0x00)
        return;

    while (((status & 0x80) == 0x80)
        && ((status & 0x01) != 0x01))
        status = commandPort.Read();

    if (status & 0x01)
    {
        printf("ERROR");
        return;
    }

    for (uint16_t i = 0; i < 256; i++)
    {
        uint16_t data = dataPort.Read();

        char* foo = "  \0";
        foo[0] = (data >> 8) & 0xFF;
        foo[1] = data & 0xFF;
        printf(foo);
    }

    printf("\n");
}

void AdvancedTechnologyAttachment::Read28(uint32_t sector, uint8_t* data, uint32_t count)
{
    // if (sector > 0x0FFFFFFF)
    //     return;
    if (sector & 0xF0000000)
        return;
    if (count > bytesPerSector)
        return;

    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1);

    lbaLowPort.Write(sector & 0x000000FF);
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHighPort.Write((sector & 0x00FF0000) >> 16);
    commandPort.Write(0x20);

    uint8_t status = commandPort.Read();
    while (((status & 0x80) == 0x80)
        && ((status & 0x01) != 0x01))
        status = commandPort.Read();

    if (status & 0x01)
    {
        printf("ERROR");
        return;
    }

    printf("Reading From ATA: ");

    for (uint16_t i = 0; i < count; i += 2)
    {
        uint16_t wdata = dataPort.Read();

        data[i] = wdata & 0x00FF;
        if(i+1 < count)
            data[i+1] = (wdata >> 8) & 0x00FF;

        // char *text = "  \0";
        // text[0] = wdata & 0xFF;

        // if(i+1 < count)
        //     text[1] = (wdata >> 8) & 0xFF;
        // else
        //     text[1] = '\0';
        
        // printf(text);
    }

    for (uint16_t i = count + (count % 2); i < bytesPerSector; i += 2)
        dataPort.Read();
}

void AdvancedTechnologyAttachment::Write28(uint32_t sector, uint8_t* data, uint32_t count)
{
    if (sector > 0x0FFFFFFF)
        return;
    if (count > 512)
        return;
    
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1);
    lbaLowPort.Write(sector & 0x000000FF);
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHighPort.Write((sector & 0x00FF0000) >> 16);
    commandPort.Write(0x30);

    printf("Writing to ATA: ");

    for (uint16_t i = 0; i < count; i += 2)
    {
        uint16_t wdata = data[i];
        if (i+1 < count)
            wdata |= ((uint16_t)data[i+1]) << 8; 

        dataPort.Write(wdata);
        
        char* foo = "  \0";
        foo[1] = (wdata >> 8) & 0x00FF;
        foo[0] = wdata & 0x00FF;
        printf(foo);
    }

    for (uint16_t i = count + (count % 2); i < 512; i += 2)
        dataPort.Write(0x0000);
}

void AdvancedTechnologyAttachment::Flush()
{
    devicePort.Write(master ? 0xE0 : 0xF0);
    commandPort.Write(0xE7);

    uint8_t status = commandPort.Read();
    if (status == 0x00)
        return;

    while (((status & 0x80) == 0x80)
        && ((status & 0x01) != 0x01))
        status = commandPort.Read();

    if (status & 0x01)
    {
        printf("ERROR");
        return;
    }
}

