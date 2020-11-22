#include "pit.h"

void InitPIT()
{
    // Set operating mode
    outb(MODE_COMMAND_REGISTER, 
        MODE_CHANNEL_0 |        // Use channel 0 PIT output
        MODE_ACCESS_LOW_HIGH |  // Send 16 bit value to 8 bit IO port with the low value first, then the high
        MODE_BINARY_16_BIT      // PIT channel will operate in binary mode
    );
}

void SetReloadValue(uint16_t value)
{
    // Set reload value in low-high mode
    outb(CHANNEL_0_DATA, value & 0xFF);
    outb(CHANNEL_0_DATA, (value >> 8) & 0xFF);
}