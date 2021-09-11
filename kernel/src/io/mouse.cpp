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
        bool bLeftButton = false;
        bool bRightButton = false;
    };
    static MouseData* pMouseData;

    static uint8_t nMouseInterrupt = 0;
    static uint8_t mouseInterruptBuffer[2];

    void Init()
    {
        // Setup mosue
        PS2::SendMouseMessage(PS2_MOUSE_USE_DEFAULTS);
        PS2::SendMouseMessage(PS2_MOUSE_ENABLE_STREAMING);

        // Change sample rate
        PS2::SendMouseMessage(PS2_MOUSE_SET_SAMPLE_RATE);
        PS2::SendMouseMessage(200);

        // Mouse data needs to have its own page
        pMouseData = (MouseData*) Memory::kPageFrame.AllocateMemory(sizeof(pMouseData), KERNEL_PAGE);

        // Install file
        *Filesystem::GetFile(Filesystem::FileDescriptors::mouse) = Filesystem::DeviceFile(sizeof(MouseData), (void*)pMouseData, "/dev/mouse", Filesystem::FileDescriptors::mouse);
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
                
                // The mouse setup will send us an extra interrupt (sometimes!). Why? Who knows!
                // But you know what? I can't be bothered fixing it! Too bad! Instead let's just
                // do some sanity checking: the first byte must have it's 4th bit (going from
                // right to left) set, so if it doesn't, that'll *probably* catch the error.
                if ((mouseInterruptBuffer[0] & MOUSE_BYTE_ALWAYS_ONE) == false)
                    return;
                
            break;

            case 1: // Mouse X 
                mouseInterruptBuffer[1] = data;
            break;

            case 2: // Mouse Y

                // It's the final byte, so update mouse
                pMouseData->deltaX = mouseInterruptBuffer[1];
                pMouseData->deltaY = data;
                pMouseData->bLeftButton = mouseInterruptBuffer[0] & MOUSE_BYTE_LEFT_BUTTON;
                pMouseData->bRightButton = mouseInterruptBuffer[0] & MOUSE_BYTE_RIGHT_BUTTON;

                // Account for signed bits (we're talking a 9-bit signed value here, don't ask me why!)
                if (pMouseData->deltaX && (mouseInterruptBuffer[0] & MOUSE_BYTE_X_SIGN_BIT)) pMouseData->deltaX |= 0xFFFFFF00;
                if (pMouseData->deltaY && (mouseInterruptBuffer[0] & MOUSE_BYTE_Y_SIGN_BIT)) pMouseData->deltaY |= 0xFFFFFF00;

                // Check we didn't move the mouse too fast and overflow
                if (mouseInterruptBuffer[0] & 0x40)
                {
                    pMouseData->deltaX = 0;
                    pMouseData->deltaY = 0;
                }

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
