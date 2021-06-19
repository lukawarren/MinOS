#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 400;
constexpr unsigned int nPadding = 5;

int main()
{
    eWindowCreate(nWidth, nHeight, "Notepad");
    ePanelCreate(nWidth-nPadding*2, nHeight-nPadding*2, nPadding, nPadding, 0xffaaaaaa);

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
                    printf("[Notepad] Unrecognised event with id %u\n", event.id);
                break;
            }
        }
        
        block();
    }
    
    eWindowClose();
    return 0;
}
