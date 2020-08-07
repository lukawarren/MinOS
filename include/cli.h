#pragma once
#ifndef CLI_H
#define CLI_H

#include <stddef.h>
#include <stdint.h>

constexpr int MAX_COMMAND_LENGTH = 100;
constexpr uint8_t CLI_BEGIN = 2;

class CLI
{
public:
    CLI(void (*_OnCommand)(char*));
    ~CLI();

    void Update(uint8_t scancode);

private:
    char commandBuffer[MAX_COMMAND_LENGTH];
    unsigned int bufferCount = CLI_BEGIN;
    void (*OnCommand)(char*);
};

#endif