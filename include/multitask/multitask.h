#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

struct Task
{
    char sName[32];
    uint32_t* pStack;
    Task* pPrevTask = nullptr;
    Task* pNextTask = nullptr;
};

Task* CreateTask(char const* sName, uint32_t entry);

extern "C"
{
    extern void SwitchToTask(Task* task);
}


#endif