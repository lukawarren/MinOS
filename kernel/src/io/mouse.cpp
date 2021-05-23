#include "io/mouse.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "filesystem/filesystem.h"
#include "filesystem/deviceFile.h"
#include "memory/memory.h"
#include "kstdlib.h"

namespace Mouse
{
    struct MouseData
    {
        int deltaX = 0;
        int deltaY = 0;
    };
    static MouseData* pMouseData;

    static uint8_t nMouseInterrupt = 0;
    static uint8_t mouseInterruptBuffer[2];

    void Init()
    {
        PS2::SendMouseMessage(PS2_MOUSE_USE_DEFAULTS);
        PS2::SendMouseMessage(PS2_MOUSE_ENABLE_STREAMING);

        // Mouse data needs to have its own page
        pMouseData = (MouseData*) Memory::kPageFrame.AllocateMemory(sizeof(pMouseData), KERNEL_PAGE);

        // Install file
        *Filesystem::GetFile(Filesystem::FileDescriptors::mouse) = Filesystem::DeviceFile(sizeof(MouseData), (void*)pMouseData);
        UART::WriteString("Actual address: ");
        UART::WriteNumber((uint32_t)pMouseData);

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

                // Check we didn't move the mouse too fast and overflow
                if (mouseInterruptBuffer[0] & MOUSE_BYTE_X_DID_OVERFLOW || mouseInterruptBuffer[0] & MOUSE_BYTE_Y_DID_OVERFLOW)
                    break;

                // It's the final byte, so update mouse
                pMouseData->deltaX = mouseInterruptBuffer[1];
                pMouseData->deltaY = data;

                // Account for signed bits (we're talking a 9-bit signed value here, don't ask me why!)
                //if (mouseInterruptBuffer[0] & MOUSE_BYTE_X_SIGN_BIT) pMouseData->deltaX |= 0xFFFFFF00;
                //if (mouseInterruptBuffer[0] & MOUSE_BYTE_Y_SIGN_BIT) pMouseData->deltaY |= 0xFFFFFF00;

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