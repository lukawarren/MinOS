#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 400;
constexpr unsigned int nPadding = 5;

int main()
{
    eWindowCreate(nWidth, nHeight, "Notepad");
    ePanelCreate(nWidth-nPadding*2, nHeight-nPadding*2, nPadding, nPadding, 0xffaaaaaa);
    
    int nChar = 0;
    
    EventLoop<sWindowManagerEvent>([&](const sWindowManagerEvent event, const bool)
    {
        switch (event.id)
        {
            case EXIT:
                return false;
                
            case WIDGET_UPDATE: // Button pressed
                return false;
            
            case KEY_DOWN:
            {    
                eKeyDown* keyEvent = (eKeyDown*) event.data;
                if (keyEvent->character == '\0') break;
                char text[2] = " ";
                text[0] = keyEvent->character;
                eTextCreate(text, nChar * CHAR_WIDTH + nPadding, nPadding);
                nChar++;
                break;
            }
            
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
