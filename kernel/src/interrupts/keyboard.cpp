#include "keyboard.h"
#include "../memory/paging.h"
#include "stdlib.h"
#include "../gfx/vga.h"
#include "../multitask/multitask.h"

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

        if (code >= 0x2 && code <= 0xa) return (char)(numbers[code - 0x2] - asciiShiftOffset);
        if(code >= 0x10 && code <= 0x1C) return (char)(qwertzuiop[code - 0x10] - asciiShiftOffset);
        else if(code >= 0x1E && code <= 0x26) return (char)(asdfghjkl[code - 0x1E] - asciiShiftOffset);
        else if(code >= 0x2C && code <= 0x32) return (char)(yxcvbnm[code - 0x2C] - asciiShiftOffset);

        return '\0'; // If key not detected, ignore it
    };
    
    scancode &= (uint8_t)(~128); // Remove released bit
    keyBuffer[(unsigned int)ScancodeToAscii(scancode)] = !released;

    //OnKeyEvent(); // Wait until the PIT interrupt to do key events, to avoid not recieving the next key interrupt (if any)
}

uint32_t GetKeyBufferAddress() { return (uint32_t) keyBuffer; }