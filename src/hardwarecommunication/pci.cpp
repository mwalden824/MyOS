#include <hardwarecommunication/pci.h>

using namespace myos::common;
using namespace myos::hardwarecommunication;
using namespace myos::drivers;

void printf(char* str);
void printfHex(uint8_t key);


PeripheralComponentInterconnectDescriptor::PeripheralComponentInterconnectDescriptor()
{

}

PeripheralComponentInterconnectDescriptor::~PeripheralComponentInterconnectDescriptor()
{

}


PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
: dataPort(0xCFC),
commandPort(0xCF8)
{

}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{
    
}

uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    uint32_t id = 
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | ((registeroffset & 0xFC));

    commandPort.Write(id);
    uint32_t result = dataPort.Read();
    return result >> (8 * (registeroffset % 4));
}

void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
    uint32_t id = 
        0x1 << 32
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | ((registeroffset & 0xFC));
    commandPort.Write(id);
    dataPort.Write(value);
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint16_t bus, uint16_t device)
{
    return Read(bus, device, 0, 0x0E) & (1<<7);
}


void PeripheralComponentInterconnectController::SelectDrivers(myos::drivers::DriverManager* driverManager)
{
    for (uint8_t bus = 0; bus < 8; bus++)
    {
        for (uint8_t device = 0; device < 32; device++)
        {
            uint8_t numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
            for (uint8_t function = 0; function < numFunctions; function++)
            {
                PeripheralComponentInterconnectDescriptor dev = GetDeviceDescriptor(bus, device, function);

                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;

                printf("PCI BUS ");
                printfHex(bus & 0xFF);

                printf(", DEVICE  ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(function & 0xFF);

                printf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);

                printf(", DEVICE ID ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                printf("\n");
            }
        }
    }
}

PeripheralComponentInterconnectDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
    PeripheralComponentInterconnectDescriptor result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0B);
    result.subclass_id = Read(bus, device, function, 0x0A);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.interruptNumber = Read(bus, device, function, 0x3C);

    return result;
}

