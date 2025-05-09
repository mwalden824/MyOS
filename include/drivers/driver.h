#ifndef __MYOS__DRIVERS__DRIVER_H
#define __MYOS__DRIVERS__DRIVER_H

namespace myos
{
    namespace drivers
    {
        class Driver
        {
            public:
                Driver();
                ~Driver();

                virtual void Activate();
                virtual int Reset();
                virtual void Deactivate();
        };

        class DriverManager
        {
            public:
                Driver* drivers[255];
                int numDrivers;

                DriverManager();
                ~DriverManager();
                void AddDriver(Driver* drv);
                void ActivateAll();
        };
    }
}

#endif