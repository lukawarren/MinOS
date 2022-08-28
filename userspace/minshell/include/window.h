#pragma once
#include "keyboard.h"
#include <stddef.h>

class Window
{
public:
    Window(unsigned int _width, unsigned int _height, void (*_draw_char)(char, unsigned int, unsigned int)) :
        width(_width), height(_height), draw_char(_draw_char) {}

    virtual void draw(const char key) = 0;
    virtual void redraw() = 0;

protected:
    unsigned int width, height;
    void (*draw_char)(char, unsigned int, unsigned int);
};


class EditorWindow : public Window
{
public:
    EditorWindow(unsigned int width, unsigned int height, void (*write_char)(char, unsigned int, unsigned int));
    ~EditorWindow();

    void draw(const char key);
    void redraw();
    char* input;

private:
    unsigned int cursor_x;
    unsigned int cursor_y;
};

class OutputWindow : public Window
{
public:
    OutputWindow(unsigned int width, unsigned int height, void (*write_char)(char, unsigned int, unsigned int));
    ~OutputWindow();

    void draw(const char) {}
    void redraw();

    char* output;
    size_t output_size;
};

class BarWindow : public Window
{
public:
    BarWindow(unsigned int width, unsigned int height, void (*write_char)(char, unsigned int, unsigned int));
    void draw(const char) {}
    void redraw() {}
};
