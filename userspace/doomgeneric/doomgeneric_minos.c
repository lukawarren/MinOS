#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "doomgeneric.h"
#include "doomkeys.h"

#include "minlib.h"
#include "client/messages.h"

static uint32_t* framebuffer;
static uint32_t initial_time = 0;

static unsigned char keyboard_state[128] = {};
static unsigned char key_buffer[256] = {};
static uint32_t key_index = 0;
static FILE* keyboard_file;

static unsigned char scancode_to_doom_key(unsigned char scancode);
static uint32_t get_ms();

void DG_Init()
{
    keyboard_file = fopen("keyboard", "r");
    assert(keyboard_file);

    // Create framebuffer and share it with PID 1 (minwm)
    size_t length = sizeof(uint32_t) * DOOMGENERIC_RESX * DOOMGENERIC_RESY;
    framebuffer = malloc(length);
    assert(share_memory(framebuffer, length, 1) == 0);

    // Create window
    struct CreateWindowMessage message = {};
    message.pid = 1;
    message.id = CREATE_WINOW_MESSAGE;
    message.width = DOOMGENERIC_RESX;
    message.height = DOOMGENERIC_RESY;
    message.framebuffer = framebuffer;
    strcpy((char*)message.title, "Doom Generic");
    send_message(&message);

    // Store initial time
    initial_time = get_ms();
}

void DG_DrawFrame()
{
    // Compare keyboard states and generate events
    unsigned char new_state[128];
    read(fileno(keyboard_file), &new_state, 128);
    for (int i = 0; i < 128; ++i)
    {
        if (new_state[i] != keyboard_state[i] && key_index <= 255)
        {
            keyboard_state[i] = new_state[i];
            key_buffer[key_index++] = i | (new_state[i] ? 0 : 0x80);
        }
    }

    // Copy output over row-by-row from DG_ScreenBuffer
    for (uint32_t y = 0; y < DOOMGENERIC_RESY; ++y)
    {
        const uint32_t offset = y * DOOMGENERIC_RESX;
        const uint32_t size = DOOMGENERIC_RESX * sizeof(framebuffer[0]);
        memcpy(framebuffer + offset, DG_ScreenBuffer + offset, size);
    }
}

void DG_SleepMs(uint32_t ms)
{
    // TODO: ammend when kernel has sleep support
    uint32_t current_ms = get_ms();
    while (1) {
        if (get_ms() - current_ms >= ms)
            break;
    }
}

uint32_t DG_GetTicksMs()
{
    return 5 * (get_ms() - initial_time);
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
    if (key_index == 0) return 0;

    // Grab oldest event
    unsigned char scancode = key_buffer[0];
    for (int i = 0; i < 255; ++i)
        key_buffer[i] = key_buffer[i+1];
    key_index--;

    *doomKey = scancode_to_doom_key(scancode & 0x7f);
    *pressed = !(scancode & 0x80);

    return 1;
}

void DG_SetWindowTitle(const char* title)
{
    struct SetWindowTitleMessage message = {};
    message.pid = 1;
    message.id = SET_WINDOW_TITLE_MESSAGE;
    strcpy((char*)message.title, title);
    send_message(&message);
}

static unsigned char scancode_to_doom_key(unsigned char scancode)
{
    unsigned char key = 0;

    switch (scancode)
    {
        case 28: return KEY_ENTER;
        case  1: return KEY_ESCAPE;
        case 30: return KEY_LEFTARROW;  // a
        case 32: return KEY_RIGHTARROW; // d
        case 17: return KEY_UPARROW;    // w
        case 31: return KEY_DOWNARROW;  // s
        case 57: return KEY_FIRE;       // space
        case 18: return KEY_USE;        // e
        case 42:
        case 54: return KEY_RSHIFT;
        case 21: return 'y';
        default: return 0;
    }

    return 0;
}

static uint32_t get_ms()
{
    struct timeval val;
    gettimeofday(&val, NULL);

    // should be "val.tv_sec * 1000", but produces stuttering
    return val.tv_sec * 100 + val.tv_usec / 1000;
}
