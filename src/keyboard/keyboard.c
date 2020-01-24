#include "keyboard.h"

#include "../std/stdout.h"
#include "../idt/idt.h"
#include "../cmd/cmd.h"
#include "../serial.h"

#define KEYBOARD_DATA 0x60
#define KEYBOARD_COMMAND 0x64

unsigned int keyboard_string_index = 0;
char keyboard_string[256];

void keyboardInterrupt(uint8_t iqr)
{
    unsigned char scanCode = inb(KEYBOARD_DATA);

    if (scanCode == ENTER_PRESSED)
    {
        stdout_setcolor(VGA_COLOR_WHITE);

        char* command = (char*) &keyboard_string;
        parseCommand(command);
        keyboard_string[0] = '\0';
        keyboard_string_index = 0;

        endIQR(iqr);
        return;
    }

    if (scanCode == ENTER_RELEASED)
    {
        keyboard_string[0] = '\0';
        keyboard_string_index = 0;
        
        initKeyboard();

        endIQR(iqr);
        return;
    }

    if (keyboardToASCII(scanCode) != '\0')
        currentKey = keyboardToASCII(scanCode);
    else
    {
        endIQR(iqr);
        return;
    }

    keyboard_string[keyboard_string_index] = currentKey;
    keyboard_string[keyboard_string_index+1] = '\0';
    ++keyboard_string_index;
   
    stdout_setcolor(VGA_COLOR_WHITE);
    printxy(keyboard_string, stdout_row, stdout_column+2);
    stdout_setcolor(VGA_COLOR_LIGHT_GREY);
    printxy("$ ", stdout_row, stdout_column);
    update_cursor(stdout_column + keyboard_string_index + 2, stdout_row);

    endIQR(iqr);
}

unsigned char keyboardToASCII(unsigned char key)
{
	
    unsigned char letter = scancodeToLetter(key);

    if (letter == BACKSPACE_PRESSED)
    {
        keyboard_string[keyboard_string_index-1] = ' ';
        keyboard_string[keyboard_string_index] = '\0';
        stdout_setcolor(VGA_COLOR_LIGHT_GREY);
        printxy("$ ", stdout_row, stdout_column);
        stdout_setcolor(VGA_COLOR_WHITE);
        printxy(keyboard_string, stdout_row, stdout_column+2);
        update_cursor(stdout_column + keyboard_string_index + 1, stdout_row);
        --keyboard_string_index;
        return '\0';
    }

    if (letter == LEFT_SHIFT_PRESSED || letter == RIGHT_SHIFT_PRESSED)
    {
        shift = 1;
        return '\0';
    }

    if (letter == LEFT_SHIFT_RELEASED || letter == RIGHT_SHIFT_RELEASED)
    {
        shift = 0;
        return '\0';
    }

    if ((letter < 'a' || letter > 'z') && letter != ' ') // if out of bounds
        return '\0';

    if (shift)
    {
        letter -= 32;
    }

    return letter;
    
}

unsigned char scancodeToLetter(unsigned char letter)
{
    switch (letter)
    {
        case A_PRESSED:
            return 'a';
        case B_PRESSED:
            return 'b';
        case C_PRESSED:
            return 'c';
        case D_PRESSED:
            return 'd';
        case E_PRESSED:
            return 'e';
        case F_PRESSED:
            return 'f';
        case G_PRESSED:
            return 'g';
        case H_PRESSED:
            return 'h';
        case I_PRESSED:
            return 'i';
        case J_PRESSED:
            return 'j';
        case K_PRESSED:
            return 'k';
        case L_PRESSED:
            return 'l';
        case M_PRESSED:
            return 'm';
        case N_PRESSED:
            return 'n';
        case O_PRESSED:
            return 'o';
        case P_PRESSED:
            return 'p';
        case Q_PRESSED:
            return 'q';
        case R_PRESSED:
            return 'r';
        case S_PRESSED:
            return 's';
        case T_PRESSED:
            return 't';
        case U_PRESSED:
            return 'u';
        case V_PRESSED:
            return 'v';
        case W_PRESSED:
            return 'w';
        case X_PRESSED:
            return 'x';
        case Y_PRESSED:
            return 'y';
        case Z_PRESSED:
            return 'z';
        case SPACE_PRESSED:
            return ' ';
        case LEFT_SHIFT_PRESSED:
            return LEFT_SHIFT_PRESSED;
        case LEFT_SHIFT_RELEASED:
            return LEFT_SHIFT_RELEASED;
        case RIGHT_SHIFT_PRESSED:
            return RIGHT_SHIFT_PRESSED;
        case RIGHT_SHIFT_RELEASED:
            return RIGHT_SHIFT_RELEASED;
        case BACKSPACE_PRESSED:
            return BACKSPACE_PRESSED;
    }
	return '\0';
}

void initKeyboard()
{
    stdout_setcolor(VGA_COLOR_LIGHT_GREY);
    printxy("$", stdout_row, stdout_column);
	update_cursor(stdout_column + 2, stdout_row);
    shift = 0;
}