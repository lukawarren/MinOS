#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 300;
constexpr unsigned int nHeight = 90;

int main()
{
    eWindowCreate(nWidth, nHeight, 0, 0, "Launcher");
    eButtonCreate("Notepad", 0, 0, nWidth);
    eButtonCreate("Launcher", 0, 30, nWidth);
    eButtonCreate("Snake", 0, 60, nWidth);

    EventLoop<sWindowManagerEvent>([&](const sWindowManagerEvent event, const bool)
    {
        switch (event.id)
        {
            case EXIT:
                return false;
            
            case WIDGET_UPDATE:
            {
                eWidgetUpdate* updateWidgetEvent = (eWidgetUpdate*) event.data;
                
                if (updateWidgetEvent->index == 0) loadprogram("notepad/notepad.bin");
                else if (updateWidgetEvent->index == 1) loadprogram("launcher/launcher.bin");
                else loadprogram("snake/snake.bin");
                
                break;
            }
            
            case KEY_DOWN:
            break;
            
            case KEY_UP:
            break;
            
            default:
                printf("[Launcher] Unrecognised event with id %u\n", event.id);
                exit(-1);
            break;
        }
        
        return true;
    });
    
    eWindowClose();
    return 0;
}
