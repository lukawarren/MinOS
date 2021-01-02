#include "keyboard.h"
#include "../memory/paging.h"
#include "../gfx/vga.h"
#include "../multitask/multitask.h"
#include "../io/uart.h"
#include "stdlib.h"
#include "task.h"

// Keyboard state
static uint8_t* keyBuffer;
static bool bShift = false;

void KeyboardInit()
{
    keyBuffer = (uint8_t*) kmalloc(256, USER_PAGE, true);
    memset(keyBuffer, 0, 256);
}

void OnKeyboardInterrupt(uint8_t scancode)
{
    bool released = scancode & 128;
    
    // Capital letters
    if (scancode == 0x2A || scancode == 0x36) { bShift = true; return; }
    else if (scancode == 0xAA || scancode == 0xB6) { bShift = false; return; }

    // Convert to ASCII char
    auto ScancodeToAscii = [&](uint8_t code)
    {
        static char const* numbers =      "123456789";
        static char const* qwertzuiop =   "qwertyuiop";
        static char const* asdfghjkl =    "asdfghjkl";
        static char const* yxcvbnm =      "zxcvbnm";

        if (code == 0x1c) return '\n';
        if (code == 0x39) return ' ';
        if (code == 0x0e) return '\r';
        if (code == 0x33 && !bShift) return ','; else if (code == 0x33) return '<';
        if (code == 0x34 && !bShift) return '.'; else if (code == 0x34) return '>';
        if (code == 0x35 && !bShift) return '/'; else if (code == 0x35) return '?';
        if (code == 0x0b) return '0';

        char asciiShiftOffset = bShift * 32; 

        if (code >= 0x2 && code <= 0xa && !bShift) return (char)(numbers[code - 0x2] - asciiShiftOffset);
        if(code >= 0x10 && code <= 0x1C) return (char)(qwertzuiop[code - 0x10] - asciiShiftOffset);
        else if(code >= 0x1E && code <= 0x26) return (char)(asdfghjkl[code - 0x1E] - asciiShiftOffset);
        else if(code >= 0x2C && code <= 0x32) return (char)(yxcvbnm[code - 0x2C] - asciiShiftOffset);

        return '\0'; // If key not detected, ignore it
    };
    
    auto ScancodeToSpecial = [&](uint8_t code)
    {
        // Special keys
        switch (code)
        {
            case 0xF:   return '\t';  // Tab
            case 0x38:  return (char) KEY_EVENT_ALT;
            case 0x48:  return (char) KEY_EVENT_UP;
            case 0x50:  return (char) KEY_EVENT_DOWN;
            case 0x4B:  return (char) KEY_EVENT_LEFT;
            case 0x4D:  return (char) KEY_EVENT_RIGHT;   
        }

        //UART::pCOM->printf("Unknown scancode ", false);
        //UART::pCOM->printf<uint32_t, true>(code);

        return '\0';
    };

    bool bSpecial = false;

    scancode &= (uint8_t)(~128); // Remove released bit
    char key = ScancodeToAscii(scancode);
    if (key == '\0') {bSpecial = true; key = ScancodeToSpecial(scancode); }
    keyBuffer[(unsigned int)key] = !released;

    if (!released) OnKeyEvent(key, bSpecial);

}

uint32_t GetKeyBufferAddress() { return (uint32_t) keyBuffer; }