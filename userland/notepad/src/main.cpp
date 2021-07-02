#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 400;

int main()
{
    eWindowCreate(nWidth, nHeight, "Notepad");
    eTextAreaCreate(0, 0, nWidth, nHeight);
    
    EventLoop<sWindowManagerEvent>([&](const sWindowManagerEvent event, const bool)
    {
        switch (event.id)
        {
            case EXIT:
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
