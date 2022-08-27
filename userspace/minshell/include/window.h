#pragma once
#include "keyboard.h"

class Window
{
public:
    Window(unsigned int _width, unsigned int _height, void (*_draw_char)(char, unsigned int, unsigned int)) :
        width(_width), height(_height), draw_char(_draw_char) {}

    virtual void draw() = 0;

protected:
    unsigned int width, height;
    void (*draw_char)(char, unsigned int, unsigned int);
};


class EditorWindow : public Window
{
public:
    EditorWindow(unsigned int width, unsigned int height, void (*write_char)(char, unsigned int, unsigned int));
    ~EditorWindow();

    void draw();

private:
    char* input;
    unsigned int cursor_x;
    unsigned int cursor_y;
    Keyboard keyboard;
};