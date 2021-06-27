#include "io/keyboard.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "filesystem/filesystem.h"
#include "filesystem/deviceFile.h"
#include "memory/memory.h"
#include "kstdlib.h"

namespace Keyboard
{
    static uint8_t* pBuffer;
    
    void Init()
    {
        // Sanity check config to check the keyboard is enabled
        uint8_t configByte = PS2::WriteAndRead(PS2_READ_CONFIG_BYTE);

        if(configByte & PS2_CONFIG_FIRST_PORT_INTERRUPT) UART::WriteString("[Keyboard] Enabled\n");
        else
        {
            UART::WriteString("[Keyboard] Disabled\n");
            assert(false);
        }
        
        pBuffer = (uint8_t*) Memory::kPageFrame.AllocateMemory(sizeof(uint8_t) * 128, KERNEL_PAGE);
        
        // Install file
        *Filesystem::GetFile(Filesystem::FileDescriptors::keyboard) = Filesystem::DeviceFile(sizeof(uint8_t) * 128, (void*)pBuffer, "/dev/keyboard", Filesystem::FileDescriptors::keyboard);
    }

    void OnInterrupt()
    {
        // Read scan code
        auto scancode = CPU::inb(PS2_DATA_PORT);
        
        // Place into buffer
        if (scancode & KEYBOARD_SCANCODE_KEY_RELEASE)
            pBuffer[scancode & (~scancode)] = 0;
        else
            pBuffer[scancode] = 1;
    }

}
