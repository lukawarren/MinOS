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
    eButtonCreate("Close window... with style", 10, nHeight - 34);

    EventLoop<sWindowManagerEvent>([&](const sWindowManagerEvent event, const bool)
    {
        switch (event.id)
        {
            case EXIT:
                return false;
                
            case WIDGET_UPDATE: // Button pressed
                return false;
            
            case KEY_DOWN:
            break;
            
            case KEY_UP:
            break;
            
            default:
                printf("[Notepad] Unrecognised event with id %u\n", event.id);
                exit(-1);
            break;
        }
        
        return true;
    });
    
    eWindowClose();
    return 0;
}
