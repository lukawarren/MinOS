#include "io/keyboard.h"
#include "cpu/cpu.h"
#include "kstdlib.h"

namespace Keyboard
{
    
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
    }

    void OnInterrupt()
    {
        // Read scan code
        CPU::inb(PS2_DATA_PORT);
    }

}