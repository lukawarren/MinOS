#pragma once
#ifndef WM_EVENTS_H
#define WM_EVENTS_H

#define CREATE_WINDOW_EVENT 0xbeefdead
#define SET_TITLE_EVENT     0xafafafaf
#define DRAW_STRING_EVENT   0xaffbeefa
#define DRAW_NUMBER_EVENT   0x10101010
#define UNBLOCK_EVENT       0xbbbbbbbb

#include "stdlib.h"
#include "interrupts/syscall.h"

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
    char message[29];
} WindowDrawString;

typedef struct WindowDrawNumber
{
    uint32_t x;
    uint32_t y;
    uint32_t colour;
    uint32_t number;
    bool hex;
} WindowDrawNumber;


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

    block();
    popLastEvent();
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

    block();
    popLastEvent();
}

void DrawWindowString(const char* string, uint32_t x, uint32_t y, uint32_t colour);
void DrawWindowString(const char* string, uint32_t x, uint32_t y, uint32_t colour)
{
    WindowDrawString message;
    message.x = x;  message.y = y; message.colour = colour;
    strncpy(message.message, string, 29);

    TaskEvent event;
    event.id = DRAW_STRING_EVENT;
    memcpy(event.data, &message, sizeof(WindowTitleMessage));
    pushEvent(getProcess("wm"), &event);

    block();
    popLastEvent();
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

    block();
    popLastEvent();
}

uint32_t GetWindowColour(uint8_t r, uint8_t g, uint8_t b);
uint32_t GetWindowColour(uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t a = 0xff;
    return a << 24 | r << 16 | g << 8 | b;
}

#define WINDOW_WHITE GetWindowColour(255, 255, 255)

#endif