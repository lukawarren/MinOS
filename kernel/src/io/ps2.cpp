#include "io/ps2.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "kstdlib.h"

namespace PS2
{
    static void WaitForOutput();
    static void WaitForInput();

    void Init()
    {
        using namespace CPU;

        // First things first, disable any PS/2 devices so they can't complain
        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_DISABLE_FIRST_PORT);
        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_DISABLE_SECOND_PORT);

        // Flush the output buffer (who knwows what happened?!)
        inb(0x60);

        // Get the configuration
        uint8_t configByte = WriteAndRead(PS2_READ_CONFIG_BYTE);

        // Sanity check
        assert(~configByte & PS2_CONFIG_MUST_BE_ZERO);

        // Disable interrupts and translation
        configByte &= (uint8_t) ~(PS2_CONFIG_FIRST_PORT_INTERRUPT | PS2_CONFIG_SECOND_PORT_INTERRUPT | PS2_CONFIG_FIRST_PORT_TRANSLATION);

        // Write to controller
        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_WRITE_CONFIG_BYTE);
        Write(PS2_DATA_PORT, configByte);

        // Perform controller self-test
        const auto controllerStatus = WriteAndRead(PS2_TEST_CONTROLLER);
        if (controllerStatus != PS2_CONTROLLER_TEST_PASSED)
        {
            UART::WriteString("[PS/2] Controller test failed\n");
            assert(false); 
        }

        // Check for two channels by enabling second port then querying configuration again
        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_ENABLE_SECOND_PORT);
        configByte = WriteAndRead(PS2_READ_CONFIG_BYTE);

        if (configByte & PS2_CONFIG_SECOND_PORT_CLOCK)
        {
            UART::WriteString("[PS/2] Second channel is not present\n");
            assert(false); 
        }

        // Disable it again before things get funky
        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_DISABLE_SECOND_PORT);

        // Check both channels
        if (WriteAndRead(PS2_TEST_FIRST_PORT) != PS2_PORT_TEST_PASSED || WriteAndRead(PS2_TEST_SECOND_PORT) != PS2_PORT_TEST_PASSED)
        {
            UART::WriteString("[PS/2] Channel tests failed\n");
            assert(false); 
        }

        // Enable devices
        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_ENABLE_FIRST_PORT);
        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_ENABLE_SECOND_PORT);

        // Enable interupts, translation and set the system flag too
        configByte |= (PS2_CONFIG_FIRST_PORT_INTERRUPT | PS2_CONFIG_SECOND_PORT_INTERRUPT | PS2_CONFIG_FIRST_PORT_TRANSLATION | PS2_CONFIG_SYSTEM_FLAG);

        // Enable clocks by clearing bits
        configByte &= (uint8_t) ~(PS2_CONFIG_FIRST_PORT_CLOCK | PS2_CONFIG_SECOND_PORT_CLOCK);

        Write(PS2_STATUS_AND_COMMAND_PORT, PS2_WRITE_CONFIG_BYTE);
        Write(PS2_DATA_PORT, configByte);
        
        // Reset keyboard
        assert(SendKeyboardMessage(PS2_RESET_DEVICE) == PS2_SELF_TEST_PASSED);

        // Reset mouse
        assert(SendMouseMessage(PS2_RESET_DEVICE) == PS2_ACK);

        UART::WriteString("[PS/2] Initialised\n");
    }

    static void WaitForOutput()
    {
        for(;;)
        {
            if((CPU::inb(PS2_STATUS_AND_COMMAND_PORT) & PS2_INPUT_BUFFER_FULL) == false)
                return;
        }
    }

    static void WaitForInput()
    {
        for(;;)
        {
            if((CPU::inb(PS2_STATUS_AND_COMMAND_PORT) & PS2_OUTPUT_BUFFER_FULL) == true)
                return;
        }
    }

    uint8_t WriteAndRead(const uint8_t value)
    {
        WaitForOutput();
        CPU::outb(PS2_STATUS_AND_COMMAND_PORT, value);
        WaitForInput();
        return CPU::inb(PS2_DATA_PORT);
    }

    void Write(const uint8_t port, const uint8_t value)
    {
        WaitForOutput();
        CPU::outb(port, value);
    }

    uint8_t SendKeyboardMessage(const uint8_t value)
    {
        WaitForOutput();
        CPU::outb(PS2_DATA_PORT, value);
        WaitForInput();
        CPU::inb(PS2_DATA_PORT);
        WaitForInput();
        return CPU::inb(PS2_DATA_PORT);
    }

    uint8_t SendMouseMessage(const uint8_t value)
    {
        WaitForOutput();
        CPU::outb(PS2_STATUS_AND_COMMAND_PORT, PS2_NEXT_BYTE_USES_SECOND_PORT);
        WaitForOutput();
        CPU::outb(PS2_DATA_PORT, value);
        WaitForInput();
        return CPU::inb(PS2_DATA_PORT);
    }

}