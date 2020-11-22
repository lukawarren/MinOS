#include "timer.h"
#include "../io/vga.h"
#include "../io/pit.h"

static int times = 0;
static int seconds = 0;

void OnTimerInterrupt()
{
    times++;
    if (times == 1000) 
    { 
        VGA_printf("Uptime: ", false);
        VGA_printf(seconds, false);
        VGA_printf(" seconds", false);
        VGA_ROW = 2;
        times = 0; 
        seconds++;
    }
    SetReloadValue(1193);
}