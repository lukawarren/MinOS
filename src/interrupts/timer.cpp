#include "timer.h"
#include "../io/vga.h"
#include "../io/pit.h"

#if DO_SOUND_DEMO
static int sampleCount = 0;
static uint16_t samples[8] = {4560, 4063, 3619, 3416, 3043, 2711, 2415, 2280};
#endif

void OnTimerInterrupt()
{
    #if DO_SOUND_DEMO
        if (sampleCount == 8) sampleCount = 0;
        outb(MODE_COMMAND_REGISTER, MODE_CHANNEL_2 | MODE_ACCESS_LOW_HIGH | MODE_OPERATING_MODE_3 | MODE_BINARY_16_BIT);
        SetReloadValueInHz(CHANNEL_2_DATA, samples[sampleCount]);
        sampleCount++;
        SetReloadValueInHz(CHANNEL_0_DATA, 20);
    #else



    #endif
    
}