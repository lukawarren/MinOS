#include "io/ps2.h"
#include "cpu/cpu.h"

namespace PS2
{

    void Init()
    {
        // First things first, disable any PS/2 devices so they can't complain
        using namespace CPU;
        outb(PS2_STATUS_AND_COMMAND_PORT, 0xAD);
        outb(PS2_STATUS_AND_COMMAND_PORT, 0xA7);
    
        // Flush the output buffer (who knwows what happened?!)
        inb(0x60);

        // Set the controller configuration byte
        Configuration configuration = {};
        
    }

}