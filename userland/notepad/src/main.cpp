#include <minlib.h>
#include "events.h"

int main()
{
    {
        sWindowManagerEvent event = sWindowManagerEvent { WINDOW_CREATE, eWindowCreate(300, 100, "Notepad") };
        Event<sWindowManagerEvent>(event, 1);
    }
    
    while(1) {}
    
    return 0;
}
