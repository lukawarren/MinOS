#pragma once
#ifndef EVENTS_H
#define EVENTS_H

#include <minlib.h>
#include "text.h"

#define WINDOW_MANAGER_PID 1

// Each message has the ID first, then the data, so that leaves
// 27 bytes per event for data

enum Events
{
    WINDOW_CREATE,
    WINDOW_CLOSE,
    PANEL_CREATE,
    TEXT_CREATE,
    BUTTON_CREATE,
    WIDGET_UPDATE,
    EXIT
};

struct sWindowManagerEvent
{
    uint8_t id;
    uint8_t data[251];
    
    sWindowManagerEvent(const uint8_t _id, const void* _data)
    {
        id = _id;
        memcpy(data, _data, sizeof(data));
    }
    
    sWindowManagerEvent(const uint8_t _id) : id(_id) {}
    
} __attribute__((packed));

struct eWindowCreate
{
    uint32_t width;
    uint32_t height;
    char sName[19];
    
    eWindowCreate(const uint32_t _width, const  uint32_t _height, const char* name) : width(_width), height(_height)
    {
        strcpy(sName, name);
        Event<sWindowManagerEvent>({WINDOW_CREATE, this}, WINDOW_MANAGER_PID);
    }
} __attribute__((packed));

struct eWindowClose
{
    eWindowClose()
    {
        Event<sWindowManagerEvent>({WINDOW_CLOSE, this}, WINDOW_MANAGER_PID);
    }
} __attribute__((packed));

struct ePanelCreate
{
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    uint32_t colour;

    ePanelCreate(const uint32_t _width, const uint32_t _height, const uint32_t _x, const uint32_t _y, const uint32_t _colour) :
        width(_width), height(_height), x(_x), y(_y), colour(_colour)
    {
        Event<sWindowManagerEvent>({PANEL_CREATE, this}, WINDOW_MANAGER_PID);
    }
} __attribute__((packed));

struct eTextCreate
{
    uint32_t x;
    uint32_t y;
    uint32_t colour;
    char text[239];

    eTextCreate(char const* _text, const uint32_t _x, const uint32_t _y, const uint32_t _colour = 0xffffffff) : x(_x), y(_y), colour(_colour)
    {
        strncpy(text, _text, sizeof(text));
        Event<sWindowManagerEvent>({TEXT_CREATE, this}, WINDOW_MANAGER_PID);
    }
} __attribute__((packed));

struct eButtonCreate
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    char text[239];

    eButtonCreate(char const* _text, const uint32_t _x, const uint32_t _y, const uint32_t _width = 0, const uint32_t _height = 24) : 
        x(_x), y(_y), width(_width != 0 ? _width : strlen(_text) * CHAR_WIDTH + 10), height(_height)
    {
        strncpy(text, _text, sizeof(text));
        Event<sWindowManagerEvent>({BUTTON_CREATE, this}, WINDOW_MANAGER_PID);
    }
} __attribute__((packed));

struct eWidgetUpdate
{
    uint32_t index;
    
    eWidgetUpdate(const uint32_t pid, const uint32_t _index) : index(_index)
    {
        Event<sWindowManagerEvent>({WIDGET_UPDATE, this}, pid);
    }
} __attribute__((packed));

struct eExit
{
    uint32_t exitCode;
    
    eExit(const uint32_t pid = 0, const uint32_t _exitCode = 0) : exitCode(_exitCode)
    {
        Event<sWindowManagerEvent>({EXIT, this}, pid == 0 ? WINDOW_MANAGER_PID : pid, false);
    }
} __attribute__((packed));

#endif
