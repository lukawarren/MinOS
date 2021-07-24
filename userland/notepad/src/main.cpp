#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 400;

int main()
{
    eWindowCreate(nWidth, nHeight, "Notepad");
    eTextAreaCreate(0, 0, nWidth, nHeight);
    
    for (int i = 0; i < 4; ++i)
    {
        eTextAreaAddRow(0, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras hendrerit", false);
        eTextAreaAddRow(0, "neque eget eros elementum placerat. Etiam blandit tellus sapien, vitae", false);
        eTextAreaAddRow(0, "tempus ligula imperdiet eget. Aenean accumsan pharetra est, rutrum", false);
        eTextAreaAddRow(0, "sodales nisl vestibulum vel. Fusce lacinia tortor sit amet eleifend", false);
        eTextAreaAddRow(0, "semper. Morbi placerat ipsum a tellus venenatis, vitae dapibus magna", false);
        eTextAreaAddRow(0, "porttitor. Sed dignissim eros sapien, in fringilla odio molestie in.", false);
        eTextAreaAddRow(0, "Aenean nec euismod nibh. Etiam nibh augue, condimentum eget dui eget,", false);
        eTextAreaAddRow(0, "posuere lobortis quam. Phasellus a tristique tellus. In egestas mi a", false);
        eTextAreaAddRow(0, "dignissim sodales. Donec vel mollis felis. Maecenas at dictum mi, in", false);
        eTextAreaAddRow(0, "tristique lorem. Donec et enim mauris. Proin sit amet lorem turpis. Duis", false);
        eTextAreaAddRow(0, "vehicula nisi eu ex tempor, at consequat magna euismod.", false);
        eTextAreaAddRow(0, "", false);
        eTextAreaAddRow(0, "", false);
    }
    eTextAreaAddRow(0, "", true);
    
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
