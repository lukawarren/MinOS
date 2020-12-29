#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"
#include "colours.h"

int main()
{
    const unsigned int width = 500;
    const unsigned int height = 400;

    CreateWindow(getFramebufferWidth() / 2 - width / 2, getFramebufferHeight() / 2 - height / 2 - 17, width, height);
    SetWindowTitle("Terminal");
    
    unsigned int nChars = 0;
    unsigned int nRows = 0;

    while (1)
    {
        TaskEvent* event = getNextEvent();
        while (event != NULL)
        {
            if (event->id == KEY_EVENT)
            {
                WindowKeyEvent* keyEvent = (WindowKeyEvent*)event->data;

                if (keyEvent->key == 0xD)  // Backspace
                { 
                    nChars--;
                    DrawWindowString(" ", nChars*8, nRows*16, WINDOW_WHITE);
                }
                else
                {
                    char message[2] = { keyEvent->key, '\0' };
                    DrawWindowString(message, nChars*8, nRows*16, WINDOW_WHITE);
                    nChars++;
                }
            }

            event = getNextEvent();
        }
    }

    sysexit();
    return 0;
}