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

void EnableScheduler();
void DisableScheduler();
void OnMultitaskPIT();

Task* CreateTask(char const* sName, uint32_t entry);

#endif