#pragma once

class Keyboard
{
public:
    Keyboard(void (*function1)(), void (*function2)());
    void poll(char* key);

private:
    bool shift;
    void (*functions[2])();
};