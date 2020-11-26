#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

struct Task
{
    char sName[32];
    uint32_t esp;
};

Task CreateTask(char* sName);

#endif