#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 600;
constexpr unsigned int nTilesPerDimension = 60;
constexpr unsigned int nTileSize = nWidth / nTilesPerDimension;

int main()
{           
    eWindowCreate(nWidth, nHeight, "Snake");

    // Init grid
    for (unsigned int x = 0; x < nTilesPerDimension; ++x)
        for (unsigned int y = 0; y < nTilesPerDimension; ++y)
            ePanelCreate(nTileSize, nTileSize, nTileSize*x, nTileSize*y, 0xffffffff);
 
    bool bRunning = true;
    while(bRunning)
    {
        // Get all events
        Pair<bool, Message> message;
        while ((message = Event<>::GetMessage()).m_first)
        {
            auto event = reinterpret_cast<sWindowManagerEvent&>(message.m_second.data);
            const uint32_t pid = message.m_second.sourcePID;
            if (pid != WINDOW_MANAGER_PID) continue;

            switch (event.id)
            {
                case EXIT:
                    bRunning = false;
                break;
                
                default:
                    printf("[Snake] Unrecognised event with id %u\n", event.id);
                    exit(-1);
                break;
            }
        }
        
        if (bRunning) block();
    }
    
    eWindowClose();
    return 0;
}
