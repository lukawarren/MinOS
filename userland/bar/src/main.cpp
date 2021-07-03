#include <minlib.h>
#include "events.h"

constexpr unsigned int nHeight = 20;

int main()
{
    const uint32_t nWidth = getscreenwidth();
    eWindowCreate(nWidth, nHeight, 0, getscreenheight() - nHeight, "Bar", false);
    
    eTextCreate("Time: ", 6, 6);

    EventLoop<sWindowManagerEvent>([&](const sWindowManagerEvent, const bool)
    {
        // Work out time
        const int systemTime = (int) time(NULL); 
        const int hours = systemTime / 60 / 60;
        const int minutes = (systemTime / 60) - (hours * 60);

        // Update text
        char sTime[32];
        sprintf(sTime, "Time: %02d:%02d\n", hours, minutes);
        eTextSet(0, sTime);

        usleep(1000000); // 1 second
        return true;
    }, false);
    
    eWindowClose();
    return 0;
}
