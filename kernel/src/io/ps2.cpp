#include "io/ps2.h"
#include "cpu/cpu.h"
#include "kstdlib.h"

namespace PS2
{
    static void WaitForOutput();
    static void WaitForInput();
    static uint8_t WriteAndRead(const uint8_t value);
    static uint8_t SendMouseMessage(const uint8_t value);

    void Init()
    {
        using namespace CPU;

        // First things first, disable any PS/2 devices so they can't complain
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_DISABLE_FIRST_PORT);
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_DISABLE_SECOND_PORT);

        // Flush the output buffer (who knwows what happened?!)
        inb(0x60);

        // Get the configuration
        uint8_t configByte = WriteAndRead(PS2_READ_CONFIG_BYTE);
        Configuration configuration = *reinterpret_cast<Configuration*>(&configByte); 

        // Sanity check
        assert(configuration.systemFlag == 1);

        // Set the controller configuration byte
        configuration.firstPortInterrupt = 0;
        configuration.secondPortInterrupt = 0;
        configuration.firstPortTranslation = 1;

        // Write to controller
        configByte = *reinterpret_cast<uint8_t*>(&configuration);
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_WRITE_CONFIG_BYTE);
        WaitForOutput();    outb(PS2_DATA_PORT, configByte);

        // Perform controller self-test
        const auto controllerStatus = WriteAndRead(PS2_TEST_CONTROLLER);
        if (controllerStatus != PS2_CONTROLLER_TEST_PASSED)
        {
            UART::WriteString("[PS/2] Controller test failed\n");
            assert(false); 
        }

        // Check for two channels by enabling second port then querying configuration again
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_ENABLE_SECOND_PORT);
        configByte = WriteAndRead(PS2_READ_CONFIG_BYTE);
        configuration = *reinterpret_cast<Configuration*>(&configByte); 

        if (configuration.secondPortClock != 0)
        {
            UART::WriteString("[PS/2] Second channel is not present\n");
            assert(false); 
        }

        // Disable it again before things get funky
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_DISABLE_SECOND_PORT);

        // Check both channels
        if (WriteAndRead(PS2_TEST_FIRST_PORT) != PS2_PORT_TEST_PASSED || WriteAndRead(PS2_TEST_SECOND_PORT) != PS2_PORT_TEST_PASSED)
        {
            UART::WriteString("[PS/2] Channel tests failed\n");
            assert(false); 
        }

        // Enable interupts
        configuration.firstPortInterrupt = 1;
        configuration.secondPortInterrupt = 1;
        configuration.firstPortClock = 0;
        configuration.secondPortClock = 0;
        configuration.firstPortTranslation = 1;
        configByte = *reinterpret_cast<uint8_t*>(&configuration);
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_WRITE_CONFIG_BYTE);
        WaitForOutput();    outb(PS2_DATA_PORT, configByte);

        // Enable devices
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_ENABLE_FIRST_PORT);
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_ENABLE_SECOND_PORT);

        // Enable interupts
        configuration.firstPortInterrupt = 1;
        configuration.secondPortInterrupt = 1;
        configuration.firstPortClock = 0;
        configuration.secondPortClock = 0;
        configuration.firstPortTranslation = 1;
        configByte = *reinterpret_cast<uint8_t*>(&configuration);
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_WRITE_CONFIG_BYTE);
        WaitForOutput();    outb(PS2_DATA_PORT, configByte);

        /*
        // Reset first device
        WaitForOutput();    outb(PS2_DATA_PORT, PS2_RESET_DEVICE);
        WaitForInput();     inb(PS2_DATA_PORT);

        // Reset second device
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_NEXT_BYTE_USES_SECOND_PORT);
        WaitForOutput();    outb(PS2_DATA_PORT, PS2_RESET_DEVICE);
        WaitForInput();     inb(PS2_DATA_PORT);
        */
        
        // Init mouse
        assert(SendMouseMessage(PS2_MOUSE_USE_DEFAULTS) == PS2_ACK);
        assert(SendMouseMessage(PS2_MOUSE_ENABLE_STREAMING) == PS2_ACK);

        UART::WriteString("[PS/2] Initialised\n");
    }

    static void WaitForOutput()
    {
        for(;;)
        {
            if((CPU::inb(PS2_STATUS_AND_COMMAND_PORT) & PS2_OUTPUT_BUFFER_FULL) == false)
                return;
        }
    }

    static void WaitForInput()
    {
        for(;;)
        {
            if((CPU::inb(PS2_STATUS_AND_COMMAND_PORT) & PS2_INPUT_BUFFER_FULL) == false)
                return;
        }
    }

    static uint8_t WriteAndRead(const uint8_t value)
    {
        WaitForOutput();
        CPU::outb(PS2_STATUS_AND_COMMAND_PORT, value);
        WaitForInput();
        return CPU::inb(PS2_DATA_PORT);
    }

    static uint8_t SendMouseMessage(const uint8_t value)
    {
        using namespace CPU;
        WaitForOutput();    outb(PS2_STATUS_AND_COMMAND_PORT, PS2_NEXT_BYTE_USES_SECOND_PORT);
        WaitForInput();     inb(PS2_DATA_PORT);
        WaitForOutput();    outb(PS2_DATA_PORT, value);
        WaitForInput();     return inb(PS2_DATA_PORT);
    }

}