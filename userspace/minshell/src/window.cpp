#include "window.h"
#include <stdlib.h>
#include <string.h>

EditorWindow::EditorWindow(unsigned int width, unsigned int height, void (*draw_char)(char, unsigned int, unsigned int)) :
        Window(width, height, draw_char)
{
    input = (char*) malloc(sizeof(char) * width * height);
}

void EditorWindow::draw()
{
    // Get key (if any)
    char key = '\0';
    keyboard.poll(&key);
    if (key == '\0') return;

    // Enter
    if (key == '\n')
    {
        if (cursor_y >= height) return;
        cursor_y++;
        cursor_x = 0;
        return;
    }

    // Backspace
    if (key == '\b')
    {
        if (cursor_x > 0)
        {
            // Erase current character...
            draw_char(' ', cursor_x-1, cursor_y);
            cursor_x--;

            // ...and remove from input
            input[cursor_x] = '\0';
        }

        return;
    }

    // Genuine keypress; too long? Too bad!
    if (cursor_x >= width) return;
    draw_char(key, cursor_x, cursor_y);
    input[cursor_y * width + cursor_x] = key;
    cursor_x++;
}

EditorWindow::~EditorWindow()
{
    free(input);
}