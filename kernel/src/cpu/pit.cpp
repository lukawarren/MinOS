#include "cpu/pit.h"
#include "cpu/cpu.h"

namespace PIT
{
    static uint16_t frequency;
    static uint16_t timerChannel;

    void Init()
    {
        // Set operating mode
        CPU::outb( MODE_COMMAND_REGISTER, 
            MODE_CHANNEL_0 |        // Use channel 0 PIT output (interrupt on terminal count)
            MODE_ACCESS_LOW_HIGH |  // Send 16 bit value to 8 bit IO port with the low value first, then the high
            MODE_OPERATING_MODE_0 | // Interrupt when reload value reaches 0
            MODE_BINARY_16_BIT      // PIT channel will operate in binary mode
        );
        
        SetFrequency(CHANNEL_0_DATA, 600);
    }

    void SetFrequency(uint16_t channel, uint16_t herz)
    {
        // The PIT's oscillator runs at about 1.193182 MHz
        const uint16_t reloadValue = (uint16_t)((uint32_t)1193180 / (uint32_t)herz);

        // As agreed, send the low bits, then the high
        CPU::outb(channel, (uint8_t) reloadValue);
        CPU::outb(channel, (uint8_t) (reloadValue >> 8));

        // Store values for resets later on
        frequency = herz;
        timerChannel = channel;
    }

    /*
        In its configured mode the PIT will only fire once, and so
        it must be continuously reset every PIT interrupt.
    */
    void Reset()
    {
        SetFrequency(timerChannel, frequency);
    }
    
    uint32_t GetDelayInMicroseconds()
    {
        return 1000000 / frequency;
    }

}
