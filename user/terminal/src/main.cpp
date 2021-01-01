#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"
#include "colours.h"
#include "font.h"

// Window dimensions
constexpr unsigned int width = 500;
constexpr unsigned int height = 300;

// Blinking cursor and prompt
constexpr unsigned int promptWidth = CHAR_WIDTH*2;
bool bCursor = false;

// Viewport dimensions
constexpr unsigned int viewportPadding = 16;
constexpr unsigned int nRows = (height - viewportPadding*2) / (CHAR_HEIGHT*CHAR_SCALE);
constexpr unsigned int nColumns = (width - promptWidth - viewportPadding*2) / CHAR_WIDTH;

// Viewport
unsigned int viewportRow = 0;
unsigned int viewportColumn = 0;
char viewportPrompt[nColumns+1];    // +1 for null terminator

int main();
void ParseKeyEvent(WindowKeyEvent* event);
void DrawPrompt();
void OnCommand();

int main()
{
    // Create window
    CreateWindow(getFramebufferWidth() / 2 - width / 2, 50, width, height);
    SetWindowTitle("Terminal");

    // Clear viewport prompt
    memset(viewportPrompt, ' ', nColumns);
    viewportPrompt[nColumns] = '\0';

    DrawPrompt();

    while (1)
    {
        // Parse events
        TaskEvent* event = getNextEvent();
        if (event != NULL)
        {
            if (event->id == KEY_EVENT)
            {
                WindowKeyEvent* keyEvent = (WindowKeyEvent*)event->data;
                ParseKeyEvent(keyEvent);
                DrawPrompt();
            }        
        }

        // Draw blinking cursor
        bCursor = getSeconds() % 2;
        DrawWindowString(bCursor ? "_" : " ", viewportPadding + (viewportColumn+2)*CHAR_WIDTH, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);
    }

    sysexit();
    return 0;
}

void ParseKeyEvent(WindowKeyEvent* event)
{
    char key = event->key;

    // Backspace
    if (key == 0xD)
    { 
        if (viewportColumn >= 1) viewportColumn--;
        
        viewportPrompt[viewportColumn] = ' ';

        return;
    }
                
    // Return
    if (key == '\n')
    { 
        OnCommand();

        return;        
    }

    if (key == '\0') return; // Unknown characters

    // Move cursor along
    viewportColumn++;

    // Constrain max command length to one line (for now)
    if (viewportColumn >= nColumns) { viewportColumn = nColumns-1; }
    else  viewportPrompt[viewportColumn-1] = key; // Update prompt
}

void DrawPrompt()
{
    // Draw cursor
    DrawWindowString(">", viewportPadding, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);
    
    // Draw command
    DrawWindowString(viewportPrompt, viewportPadding + promptWidth, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);
}

void OnCommand()
{
    // Force cursor to be off
    DrawWindowString(" ", viewportPadding + (viewportColumn+2)*CHAR_WIDTH, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);

    // Increment row
    viewportRow ++;
    viewportColumn = 0;
    if (viewportRow >= nRows) viewportRow = 0;

    // Draw stdout
    DrawWindowString("Unknown command", viewportPadding + promptWidth, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);

    // Increment row again
    viewportRow++;
    if (viewportRow >= nRows) viewportRow = 0;

    // Clear prompt
    memset(viewportPrompt, ' ', nColumns);
    viewportPrompt[nColumns] = '\0';
}