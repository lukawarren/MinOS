#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 300;
constexpr unsigned int nHeight = 100;
constexpr unsigned int nPadding = 5;

int main()
{
    eWindowCreate(nWidth, nHeight, 0, 0, "Launcher");
    eButtonCreate("Notepad", nPadding, nPadding, nWidth - nPadding * 2);
    eButtonCreate("Launcher", nPadding, nPadding + 30, nWidth - nPadding * 2);
    eButtonCreate("Snake", nPadding, nPadding + 60, nWidth - nPadding * 2);

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
