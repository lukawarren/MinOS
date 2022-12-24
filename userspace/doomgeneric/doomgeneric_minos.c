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
#include "messages.h"

static uint32_t* framebuffer;
static uint32_t initial_time = 0;
static char key_buffer[256] = {};
static uint32_t key_index = 0;
static FILE* keyboard_file;

static unsigned char scancode_to_doom_key(unsigned char scancode);
static uint32_t get_ms();

void DG_Init()
{
    keyboard_file = fopen("keyboard", "r");
    assert(keyboard_file);

    // Create framebuffer
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
    assert(add_messages(&message, 1) == 1);

    // Store initial time
    initial_time = get_ms();
}

void DG_DrawFrame()
{
    // Add input to queue
    char scancode = 0;
    read(fileno(keyboard_file), &scancode, 1);
    if (scancode != -1 && scancode != 0 && key_index < 256)
        key_buffer[++key_index] = scancode;

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

    char scancode = key_buffer[key_index--];

    // Temporary minwm hack
    if (scancode == 15)
    {
        struct SwitchWindowMessage message = {};
        message.pid = 1;
        message.id = SWITCH_WINOW_MESSAGE;
        assert(add_messages(&message, 1) == 1);
        return 0;
    }

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
    assert(add_messages(&message, 1) == 1);

}

static unsigned char scancode_to_doom_key(unsigned char scancode)
{
    unsigned char key = 0;

    switch (scancode)
    {
    case 28:
        key = KEY_ENTER;
        break;
    case 1:
        key = KEY_ESCAPE;
        break;
    case 30: // a
        key = KEY_LEFTARROW;
        break;
    case 32: // d
        key = KEY_RIGHTARROW;
        break;
    case 17: // w
        key = KEY_UPARROW;
        break;
    case 31: // s
        key = KEY_DOWNARROW;
        break;
    case 57: // space
        key = KEY_FIRE;
        break;
    case 18: // e
        key = KEY_USE;
        break;
    case 42:
    case 54:
        key = KEY_RSHIFT;
        break;
    case 21:
        key = 'y';
        break;
    default:
        break;
    }

    return key;
}

static uint32_t get_ms()
{
    struct timeval val;
    gettimeofday(&val, NULL);

    // should be "val.tv_sec * 1000", but produces stuttering
    return val.tv_sec * 100 + val.tv_usec / 1000;
}
