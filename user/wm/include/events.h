#pragma once
#ifndef WM_EVENTS_H
#define WM_EVENTS_H

#define CREATE_WINDOW_EVENT 0xbeefdead
#define SET_TITLE_EVENT     0xafafafaf

typedef struct WindowCreateMessage
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} WindowCreateMessage;

typedef struct WindowTitleMessage
{
    char title[16];
} WindowTitleMessage;

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
}

void SetWindowTitle(const char* sTitle);
void SetWindowTitle(const char* sTitle)
{
    WindowTitleMessage message;
    strncpy(message.title, sTitle, 16);

    TaskEvent event;
    event.id = SET_TITLE_EVENT;
    memcpy(event.data, &message, sizeof(WindowTitleMessage));
    pushEvent(getProcess("wm"), &event);
}

#endif