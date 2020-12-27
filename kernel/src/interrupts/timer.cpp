#include "timer.h"
#include "../gfx/vga.h"
#include "../io/pit.h"
#include "../multitask/multitask.h"

#if DO_SOUND_DEMO
static int sampleCount = 0;
static int soundDelayCount = 0;
static uint16_t samples[8] = {4560, 4063, 3619, 3416, 3043, 2711, 2415, 2280};
#endif

static int nSeconds = 0;
static int nSubseconds = 0;

void OnTimerInterrupt()
{
    #if DO_SOUND_DEMO
        if (soundDelayCount == 50)
        {
            if (sampleCount == 8) sampleCount = 0;
            outb(MODE_COMMAND_REGISTER, MODE_CHANNEL_2 | MODE_ACCESS_LOW_HIGH | MODE_OPERATING_MODE_3 | MODE_BINARY_16_BIT);
            SetReloadValueInHz(CHANNEL_2_DATA, samples[sampleCount]);
            sampleCount++;
            soundDelayCount = 0;
        }
        soundDelayCount++;
    #endif

    SetReloadValueInHz(CHANNEL_0_DATA, 60);
    nSubseconds++;
    if (nSubseconds == 60)
    {
        nSubseconds = 0;
        nSeconds++;
    }

    // Multitasking
    OnMultitaskPIT();
}

uint32_t GetSeconds() { return nSeconds; }