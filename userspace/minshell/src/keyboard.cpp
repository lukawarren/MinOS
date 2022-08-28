#include "keyboard.h"
#include <unistd.h>
#include <stdio.h>

Keyboard::Keyboard(void (*function1)(), void (*function2)(), void (*function3)()) : shift(false)
{
    functions[0] = function1;
    functions[1] = function2;
    functions[2] = function3;
}

void Keyboard::poll(char* key)
{
    // Read key from kernel (if any)
    char scancode;
    read(3, &scancode, sizeof(char));
    if (scancode == 0) return;

    // Shifts
    if (scancode == 42 || scancode == -86)
    {
        shift = !shift;
        return;
    }

    // F1, F2, etc.
    if (scancode == 59) { functions[0](); return; }
    if (scancode == 60) { functions[1](); return; }
    if (scancode == 61) { functions[2](); return; }

    // Released keys
    if (scancode & 0x80) return;

    // TODO: convert to lookup table
    const auto to_char = [&]()
    {
        static char const* topNumbers =   "!\"#$%^&*()_+";
        static char const* numbers =      "1234567890-=";
        static char const* qwertzuiop =   "qwertyuiopQWERTYUIOP";
        static char const* asdfghjkl =    "asdfghjklASDFGHJKL";
        static char const* yxcvbnm =      "zxcvbnmZXCVBNM";

        // Numbers and symbols
        if (scancode >= 0x2 && scancode <= 0xd)
        {
            if (shift) return topNumbers[scancode - 0x2];
            else return numbers[scancode - 0x2];
        }

        // Standard letters
        else if(scancode >= 0x10 && scancode <= 0x1b) return qwertzuiop[scancode - 0x10 + shift*10];
        else if(scancode >= 0x1E && scancode <= 0x26) return asdfghjkl[scancode - 0x1E + shift*9];
        else if(scancode >= 0x2C && scancode <= 0x32) return yxcvbnm[scancode - 0x2C + shift*7];

        // Special characters - TODO: do as per above
        else if (scancode == 57) return ' ';
        else if (scancode == 51) return shift ? '<' : ',';
        else if (scancode == 52) return shift ? '>' : '.';
        else if (scancode == 28) return '\n';
        else if (scancode == 14) return '\b';
        else if (scancode == 39) return ';';

        printf("[minshell] unmapped scancode %d\n", scancode);
        return '\0';
    };

    *key = to_char();
}