#ifndef __MYOS__DRIVERS__ATA__H
#define __MYOS__DRIVERS__ATA__H

#include <hardwarecommunication/port.h>
#include <common/types.h>

namespace myos
{
    namespace drivers
    {
        class AdvancedTechnologyAttachment
        {
            protected:
                myos::hardwarecommunication::Port16Bit dataPort;
                myos::hardwarecommunication::Port8Bit errorPort;
                myos::hardwarecommunication::Port8Bit sectorCountPort;
                myos::hardwarecommunication::Port8Bit lbaLowPort;
                myos::hardwarecommunication::Port8Bit lbaMidPort;
                myos::hardwarecommunication::Port8Bit lbaHighPort;
                myos::hardwarecommunication::Port8Bit devicePort;
                myos::hardwarecommunication::Port8Bit commandPort;
                myos::hardwarecommunication::Port8Bit controlPort;

                bool master;
                myos::common::uint16_t bytesPerSector;
            public:
                AdvancedTechnologyAttachment(myos::common::uint16_t portBase, bool master);
                ~AdvancedTechnologyAttachment();

                void Identify();
                void Read28(myos::common::uint32_t sector, myos::common::uint8_t* mbr, myos::common::uint32_t count);
                void Write28(myos::common::uint32_t sector, myos::common::uint8_t* data, myos::common::uint32_t count);
                void Flush();

        };
    }
}

#endif