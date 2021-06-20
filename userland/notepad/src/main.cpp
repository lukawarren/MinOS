#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 400;
constexpr unsigned int nPadding = 5;

int main()
{
    eWindowCreate(nWidth, nHeight, "Notepad");
    ePanelCreate(nWidth-nPadding*2, nHeight-nPadding*2, nPadding, nPadding, 0xffaaaaaa);
    
    eTextCreate("Hello and welcome to MinOS! This piece of text may not have been typed ", nPadding * 2, nPadding * 2);
    eTextCreate("manually, but I am sure given enough time, the keyboard driver will see", nPadding * 2, nPadding * 4);
    eTextCreate("to that :-)                                                            ", nPadding * 2, nPadding * 6);

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
                    printf("[Notepad] Exiting...\n");
                break;
                
                case 65: // ACK
                break;
                
                default:
                    printf("[Notepad] Unrecognised event with id %u\n", event.id);
                break;
            }
        }
        
        if (bRunning) block();
    }
    
    eWindowClose();
    return 0;
}
