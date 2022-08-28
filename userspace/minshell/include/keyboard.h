#pragma once

class Keyboard
{
public:
    Keyboard(void (*function1)(), void (*function2)(), void (*function3)());
    void poll(char* key);

private:
    bool shift;
    void (*functions[3])();
};