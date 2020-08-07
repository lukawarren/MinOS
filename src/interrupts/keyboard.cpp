#include "keyboard.h"
#include "../io/vga.h"

Keyboard::Keyboard(CLI* _cli)
{
    for (size_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); ++i) buffer[i] = 0;
    cli = _cli;
}

void Keyboard::OnKeyUpdate(uint8_t scancode)
{
    // Shift for capital letters
    if (scancode == 0x2A || scancode == 0x36) { shift = true; return; }
    else if (scancode == 0xAA || scancode == 0xB6) { shift = false; return; }

    // Work out if key pressed
    auto IsReleasedScancode = [](uint8_t code)
    {
        return (int((code / pow(16, 1)) % 16) == 0x9) 
                || (int((code / pow(16, 1)) % 16) == 0xA) 
                || (int((code / pow(16, 1)) % 16) == 0xB);
    };

    // If key released, remove from buffer, else set it
    if (IsReleasedScancode(scancode)) { buffer[scancode-0x80] = 0; return; } 
    else buffer[scancode] = 1;

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
        if (code == 0x33 && !shift) return ','; else if (code == 0x33) return '<';
        if (code == 0x34 && !shift) return '.'; else if (code == 0x34) return '>';
        if (code == 0x35 && !shift) return '/'; else if (code == 0x35) return '?';
        if (code == 0x0b) return '0';

        char asciiShiftOffset = shift * 32; 

        if (code >= 0x2 && code <= 0xa) return (char)(numbers[code - 0x2] - asciiShiftOffset);
        if(code >= 0x10 && code <= 0x1C) return (char)(qwertzuiop[code - 0x10] - asciiShiftOffset);
        else if(code >= 0x1E && code <= 0x26) return (char)(asdfghjkl[code - 0x1E] - asciiShiftOffset);
        else if(code >= 0x2C && code <= 0x32) return (char)(yxcvbnm[code - 0x2C] - asciiShiftOffset);

        return '\0'; // If key not detected, ignore it
    };

    cli->Update(ScancodeToAscii(scancode));
}

Keyboard::~Keyboard() { }