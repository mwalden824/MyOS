#ifndef __MYOS__DRIVERS__VGA_H
#define __MYOS__DRIVERS__VGA_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {
        class VideoGraphicsArray
        {
            protected:
                myos::hardwarecommunication::Port8Bit miscPort;
                myos::hardwarecommunication::Port8Bit crtcIndexPort;
                myos::hardwarecommunication::Port8Bit crtcDataPort;
                myos::hardwarecommunication::Port8Bit sequencerIndexPort;
                myos::hardwarecommunication::Port8Bit sequencerDataPort;
                myos::hardwarecommunication::Port8Bit graphicsControllerIndexPort;
                myos::hardwarecommunication::Port8Bit graphicsControllerDataPort;
                myos::hardwarecommunication::Port8Bit attributeControllerIndexPort;
                myos::hardwarecommunication::Port8Bit attributeControllerReadPort;
                myos::hardwarecommunication::Port8Bit attributeControllerWritePort;
                myos::hardwarecommunication::Port8Bit attributeControllerResetPort;

                void WriteRegisters(myos::common::uint8_t* registers);
                myos::common::uint8_t* GetFrameBufferSegment();

                virtual myos::common::uint8_t GetColorIndex(myos::common::uint8_t red, myos::common::uint8_t green, myos::common::uint8_t blue);

            public:
                VideoGraphicsArray();
                ~VideoGraphicsArray();

                virtual bool SupportsMode(myos::common::uint32_t width, myos::common::uint32_t height, myos::common::uint32_t depth);
                virtual bool SetMode(myos::common::uint32_t width, myos::common::uint32_t height, myos::common::uint32_t depth);

                virtual void PutPixel(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t colorIndex);
                virtual void PutPixel(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t red, myos::common::uint8_t green, myos::common::uint8_t blue);

                virtual void FillRectangle(myos::common::uint32_t x, myos::common::uint32_t y, myos::common::uint32_t w, myos::common::uint32_t h, myos::common::uint8_t red, myos::common::uint8_t green, myos::common::uint8_t blue);
        };
    }
}

#endif