#include "window.h"
#include <stdlib.h>
#include <string.h>

EditorWindow::EditorWindow(unsigned int width, unsigned int height, void (*draw_char)(char, unsigned int, unsigned int)) :
        Window(width, height, draw_char)
{
    const size_t size = sizeof(char) * width * height;
    input = (char*) malloc(size);
    memset(input, 0, size);
}

void EditorWindow::draw(const char key)
{
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

void EditorWindow::redraw()
{
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x)
            draw_char(input[y * width + x], x, y);
}

EditorWindow::~EditorWindow()
{
    free(input);
}


OutputWindow::OutputWindow(unsigned int width, unsigned int height, void (*draw_char)(char, unsigned int, unsigned int)) :
        Window(width, height, draw_char)
{
    output_size = sizeof(char) * width * height;
    output = (char*) malloc(output_size);
    memset(output, ' ', output_size);
}

void OutputWindow::redraw()
{
    // Clear window
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x)
            draw_char(' ', x, y);

    unsigned int x = 0;
    unsigned int y = 0;
    size_t i = 0;

    while (output[i] != '\0')
    {
        draw_char(output[i], x++, y);

        // Wrap output
        if (x >= width || output[i] == '\n') {
            x = 0;
            y++;
        }
        i++;

        // No scrolling for now
        if (y >= height) break;
    }
}

OutputWindow::~OutputWindow()
{
    free(output);
}


BarWindow::BarWindow(unsigned int width, unsigned int height, void (*draw_char)(char, unsigned int, unsigned int)) :
        Window(width, height, draw_char)
{
    for (unsigned int i = 0; i < width; ++i)
        draw_char(' ', i, 0);

    const char* title = "F1 - Editor    F2 - Output    F3 - Run";
    const auto len = strlen(title);
    for (size_t i = 0; i < len; ++i)
        draw_char(title[i], width/2-len/2+i, 0);
}
