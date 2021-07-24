#include <minlib.h>
#include "events.h"

constexpr unsigned int nHeight = 20;
constexpr unsigned int nPadding = nHeight / 2 - CHAR_HEIGHT / 2;

int main()
{
    const uint32_t nWidth = getscreenwidth();
    eWindowCreate(nWidth, nHeight, 0, getscreenheight() - nHeight, "Bar", false);
    
    eTextCreate("Time: ", nPadding, nPadding);
    eTextCreate("Memory: 000 MB / 000 MB", nWidth - strlen("Memory: 000 MB / 000 MB") * CHAR_WIDTH - nPadding, nPadding);

    EventLoop<sWindowManagerEvent>([&](const sWindowManagerEvent, const bool)
    {
        // Work out time
        const int systemTime = (int) time(NULL); 
        const int hours = systemTime / 60 / 60;
        const int minutes = (systemTime / 60) - (hours * 60);

        // Update text
        char sTime[32];
        sprintf(sTime, "Time: %02d:%02d", hours, minutes);
        eTextSet(0, sTime);
        
        // Update memory info
        char sMemory[64];
        sprintf(sMemory, "Memory: %03zu MB / %03zu MB", getusedmemory() / 1024 / 1024, gettotalmemory() / 1024 / 1024);
        eTextSet(1, sMemory);

        usleep(1000000);
        return true;
    }, false);
    
    eWindowClose();
    return 0;
}
