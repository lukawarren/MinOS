#pragma once

class Keyboard
{
public:
    Keyboard();
    void poll(char* key);

private:
    bool shift;
};