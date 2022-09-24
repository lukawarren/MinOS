#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

#include "doomgeneric.h"
#include "doomkeys.h"

#include "minlib.h"
#include "messages.h"

const uint32_t framebuffer_width = 640;
const uint32_t framebuffer_height = 480;
const uint32_t framebuffer_x = (framebuffer_width - DOOMGENERIC_RESX) / 2 + 5;
const uint32_t framebuffer_y = (framebuffer_height - DOOMGENERIC_RESY) / 2 + 5;

static uint32_t* framebuffer = (uint32_t*)0x30000000;
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

    // Create window
    struct CreateWindowMessage message = {};
    message.pid = 1;
    message.id = CREATE_WINOW_MESSAGE;
    message.width = DOOMGENERIC_RESX;
    message.height = DOOMGENERIC_RESY;
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
        uint32_t fb_offset = (framebuffer_y+y)*framebuffer_width + framebuffer_x;
        uint32_t dg_offset = y*DOOMGENERIC_RESX;
        uint32_t size = DOOMGENERIC_RESX*sizeof(framebuffer[0]);
        memcpy(framebuffer + fb_offset, DG_ScreenBuffer + dg_offset, size);
    }
}

void DG_SleepMs(uint32_t ms)
{
    return;
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
    *doomKey = scancode_to_doom_key(scancode & 0x7f);
    *pressed = !(scancode & 0x80);

    return 1;
}

void DG_SetWindowTitle(const char* title) {}

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
