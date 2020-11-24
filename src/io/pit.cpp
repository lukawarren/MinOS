#include "pit.h"

void InitPIT()
{
    // Set operating mode
    outb(MODE_COMMAND_REGISTER, 
        MODE_CHANNEL_0 |        // Use channel 0 PIT output
        MODE_ACCESS_LOW_HIGH |  // Send 16 bit value to 8 bit IO port with the low value first, then the high
        MODE_OPERATING_MODE_0 | // Interrupt when reload value reaches 0
        MODE_BINARY_16_BIT      // PIT channel will operate in binary mode
    );
    
    SetReloadValueInHz(CHANNEL_0_DATA, 8000);
}

void SetReloadValue(uint16_t channel, uint16_t value)
{
    // Set reload value in low-high mode
    outb(channel, (uint8_t) value);
    outb(channel, (uint8_t) (value >> 8));
}

void SetReloadValueInHz(uint16_t channel, uint16_t value)
{
    SetReloadValue(channel, (uint16_t)((uint32_t)1193180 / (uint32_t)value));
}

void EnablePCSpeaker()
{
    // Set bit 1 of port 0x61 on the keyboard controller to connect
    // it to timer number 2
    uint8_t speakerBits = inb(PC_SPEAKER_IO_PORT);
    speakerBits |= PC_SPEAKER_ENABLE;
    outb(PC_SPEAKER_IO_PORT, speakerBits);
}

void DisablePCSpeaker()
{
    uint8_t speakerBits = inb(PC_SPEAKER_IO_PORT);
    speakerBits &= PC_SPEAKER_DISABLE;
    outb(PC_SPEAKER_IO_PORT, speakerBits);
}