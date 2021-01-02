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

// Running program
bool bInCommand = false;
uint32_t processID = 0;

int main();
void ParseKeyEvent(WindowKeyEvent* event);
void DrawPrompt();
void OnCommand();

int main()
{
    // Subscribe to evnts
    subscribeToStdout(true);
    subscribeToSysexit(true);

    // Get access to keyboard buffer for keyboard shortcuts
    uint8_t* pKeyboardBuffer = (uint8_t*) getKeyBufferAddr();

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
            if (event->id == KEY_EVENT && !bInCommand)
            {
                WindowKeyEvent* keyEvent = (WindowKeyEvent*)event->data;
                ParseKeyEvent(keyEvent);
                if (!bInCommand) DrawPrompt();
            }

            else if (event->id == KEY_EVENT && bInCommand)
            {
                // CTRL + C kills the running program
                WindowKeyEvent* keyEvent = (WindowKeyEvent*)event->data;
                if (pKeyboardBuffer[KEY_EVENT_CTRL] && keyEvent->key == 'c') kill(processID);
                
                bInCommand = false;

                // Increment row again
                viewportRow++;
                if (viewportRow >= nRows) viewportRow = 0;
                viewportColumn = 0;

                // Clear prompt
                memset(viewportPrompt, ' ', nColumns);
                viewportPrompt[nColumns] = '\0';
                
                // Draw prompt
                bInCommand = false;
                DrawPrompt();
            }

            else if (event->id == EVENT_QUEUE_PRINTF && bInCommand)
            {
                char* string = (char*)event->data;
                
                // Find if string contains newlines
                bool bContainsNewlines = false;
                for (uint32_t i = 0; i < strlen(string); ++i)
                    if (string[i] == '\n') bContainsNewlines = true;

                if (bContainsNewlines)
                {
                    /*
                        The below code only supports
                        up to one newline in a row because
                        I am writing this after 6 hours of sleep.
                        Too bad!
                    */

                    // March until newline, then print
                    uint32_t nChars = 0;
                    uint32_t segmentBegin = 0;
                    while (nChars < strlen(string))
                    {
                        // If next character is a newline, print it and advance
                        if (string[nChars+1] == '\n')
                        {
                            string[nChars+1] = '\0';

                            DrawWindowString(string + segmentBegin, viewportPadding + promptWidth + viewportColumn*CHAR_WIDTH, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);
                            viewportColumn = 0;
                            viewportRow++;
                            if (viewportRow >= nRows) viewportRow = 0;
                            
                            string[nChars+1] = '\n';

                            nChars++;
                            segmentBegin = nChars+1;
                        }

                        nChars++;
                    }
                }
                else
                {
                    DrawWindowString(string, viewportPadding + promptWidth + viewportColumn*CHAR_WIDTH, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);
                    viewportColumn += strlen(string);
                }
            }

            else if (event->id == EVENT_QUEUE_SYSEXIT && bInCommand)
            {
                // Increment row again
                viewportRow++;
                if (viewportRow >= nRows) viewportRow = 0;
                viewportColumn = 0;

                // Clear prompt
                memset(viewportPrompt, ' ', nColumns);
                viewportPrompt[nColumns] = '\0';
                
                // Draw prompt
                bInCommand = false;
                DrawPrompt();
            }        
        }

        // Draw blinking cursor
        if (!bInCommand)
        {
            bCursor = getSeconds() % 2;
            DrawWindowString(bCursor ? "_" : " ", viewportPadding + (viewportColumn+2)*CHAR_WIDTH, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);
        }
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

    // Get program name
    char programName[nColumns+1];
    strncpy(programName, viewportPrompt, nColumns+1);
    programName[viewportColumn] = '\0';

    // Increment row
    viewportRow++;
    viewportColumn = 0;
    if (viewportRow >= nRows) viewportRow = 0;

    // Run program
    bInCommand = true;
    processID = loadProgram(programName);
    if ((int) processID == -1)
    {
        // "In-program" commands
        if (strcmp(programName, "exit")) sysexit();
        else DrawWindowString("Failed to find program", viewportPadding + promptWidth, viewportPadding + viewportRow*CHAR_HEIGHT*CHAR_SCALE, WINDOW_WHITE);

        // Increment row again
        viewportRow++;
        if (viewportRow >= nRows) viewportRow = 0;

        // Clear prompt
        memset(viewportPrompt, ' ', nColumns);
        viewportPrompt[nColumns] = '\0';

        bInCommand = false;
    }
}