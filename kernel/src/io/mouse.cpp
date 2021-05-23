#include "io/mouse.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "filesystem/filesystem.h"
#include "filesystem/deviceFile.h"
#include "kstdlib.h"

namespace Mouse
{
    struct MouseData
    {
        int mouseX = 0;
        int mouseY = 0;
    };
    static MouseData mouseData = {};

    static uint8_t nMouseInterrupt = 0;
    static uint8_t mouseInterruptBuffer[2];

    void Init()
    {
        PS2::SendMouseMessage(PS2_MOUSE_USE_DEFAULTS);
        PS2::SendMouseMessage(PS2_MOUSE_ENABLE_STREAMING);

        // Install file
        *Filesystem::GetFile(Filesystem::FileDescriptors::mouse) = Filesystem::DeviceFile(sizeof(mouseData), (void*)&mouseData);
        
        UART::WriteString("[Mouse] Initialised\n");
    }

    void OnInterrupt()
    {
        // Read scan code
        const uint8_t data = CPU::inb(PS2_DATA_PORT);
        
        // We recieve 3 bits per "chunk" of data, each with a different meaning
        switch (nMouseInterrupt)
        {
            case 0: // Various pieces of information
                mouseInterruptBuffer[0] = data;
            break;

            case 1: // Mouse X 
                mouseInterruptBuffer[1] = data;
            break;

            case 2: // Mouse Y

                // Final byte, update mouse
                mouseData.mouseX = mouseInterruptBuffer[1];
                mouseData.mouseY = data;

                // Account for signed bits
                if (mouseInterruptBuffer[0] & MOUSE_BYTE_X_SIGN_BIT) mouseData.mouseX |= 0xFFFFFF00;
                if (mouseInterruptBuffer[0] & MOUSE_BYTE_Y_SIGN_BIT) mouseData.mouseY |= 0xFFFFFF00;

            break;

            default:
                assert(false);
            break;
        }

        // Loop back around for next time
        nMouseInterrupt++;
        if (nMouseInterrupt > 2) nMouseInterrupt = 0;
    }

}