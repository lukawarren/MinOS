#include <minlib.h>
#include "events.h"

int main()
{
    eWindowCreate(600, 400, "Notepad");
    ePanelCreate(600-10, 400-10, 5, 5, 0xffffffff);

    while(1) {}
    
    return 0;
}
