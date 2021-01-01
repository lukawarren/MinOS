#pragma once
#ifndef WM_EVENTS_H
#define WM_EVENTS_H

#define CREATE_WINDOW_EVENT 0xbeefdead
#define SET_TITLE_EVENT     0xafafafaf
#define DRAW_STRING_EVENT   0xaffbeefa
#define DRAW_NUMBER_EVENT   0x10101010
#define UNBLOCK_EVENT       0xbbbbbbbb
#define KEY_EVENT           0xfacefeed

#include "stdlib.h"
#include "interrupts/syscall.h"
#include "font.h"

typedef struct WindowCreateMessage
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} WindowCreateMessage;

typedef struct WindowTitleMessage
{
    char title[32];
} WindowTitleMessage;

typedef struct WindowDrawString
{
    uint32_t x;
    uint32_t y;
    uint32_t colour;
    char message[20];
} WindowDrawString;

typedef struct WindowDrawNumber
{
    uint32_t x;
    uint32_t y;
    uint32_t colour;
    uint32_t number;
    bool hex;
} WindowDrawNumber;

typedef struct WindowKeyEvent
{
    char key;
} WindowKeyEvent;

void CreateWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void CreateWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    WindowCreateMessage message;
    message.x = x;
    message.y = y;
    message.width = width;
    message.height = height;

    TaskEvent event;
    event.id = CREATE_WINDOW_EVENT;
    memcpy(event.data, &message, sizeof(WindowCreateMessage));
    pushEvent(getProcess("wm"), &event);

    blockUntil(UNBLOCK_EVENT);
    popLastEvent(UNBLOCK_EVENT);
}

void SetWindowTitle(const char* sTitle);
void SetWindowTitle(const char* sTitle)
{
    WindowTitleMessage message;
    strncpy(message.title, sTitle, 32);

    TaskEvent event;
    event.id = SET_TITLE_EVENT;
    memcpy(event.data, &message, sizeof(WindowTitleMessage));
    pushEvent(getProcess("wm"), &event);

    blockUntil(UNBLOCK_EVENT);
    popLastEvent(UNBLOCK_EVENT);
}

void DrawWindowString(const char* string, uint32_t x, uint32_t y, uint32_t colour);
void DrawWindowString(const char* string, uint32_t x, uint32_t y, uint32_t colour)
{
    WindowDrawString message;
    message.x = x;  message.y = y; message.colour = colour;

    // Dispatch events in the form of 19 chars at a time (plus null terminator)
    uint32_t nEvents = 0;
    for (uint32_t i = 0; i < strlen(string); i+=19)
    {
        // Create event and fill parameters
        TaskEvent event;
        event.id = DRAW_STRING_EVENT;
        memcpy(event.data, &message, sizeof(uint32_t)*3);
        
        // Fill string data
        for (uint32_t c = 0; c < 19; ++c)
        {
            event.data[sizeof(uint32_t)*3 + c] = (uint8_t) string[i+c];
            //if (string[i+c] == '\0') break; // Break if null terminator
        }
        event.data[sizeof(uint32_t)*3 + 19] = '\0';

        // Increment x
        message.x += 19*CHAR_WIDTH;

        pushEvent(getProcess("wm"), &event);
        nEvents++;
    }

    blockUntil(UNBLOCK_EVENT);
    for (uint32_t i = 0; i < nEvents; ++i) popLastEvent(UNBLOCK_EVENT);
}

void DrawWindowNumber(uint32_t number, uint32_t x, uint32_t y, uint32_t colour, bool hex);
void DrawWindowNumber(uint32_t number, uint32_t x, uint32_t y, uint32_t colour, bool hex)
{
    WindowDrawNumber message;
    message.x = x;
    message.y = y; 
    message.colour = colour; 
    message.hex = hex;
    message.number = number;

    TaskEvent event;
    event.id = DRAW_NUMBER_EVENT;
    memcpy(event.data, &message, sizeof(WindowDrawNumber));
    pushEvent(getProcess("wm"), &event);

    blockUntil(UNBLOCK_EVENT);
    popLastEvent(UNBLOCK_EVENT);
}

uint32_t GetWindowColour(uint8_t r, uint8_t g, uint8_t b);
uint32_t GetWindowColour(uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t a = 0xff;
    return a << 24 | r << 16 | g << 8 | b;
}

#define WINDOW_WHITE GetWindowColour(255, 255, 255)

#endif