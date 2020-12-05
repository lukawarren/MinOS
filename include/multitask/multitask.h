#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

#include "../memory/tss.h"

struct Task
{
    char sName[32];
    uint32_t* pStack;
    Task* pPrevTask = nullptr;
    Task* pNextTask = nullptr;
};

void SetTSSForMultitasking(TSS* _tss);

void EnableScheduler();
void DisableScheduler();

void OnMultitaskPIT();

Task* CreateTask(char const* sName, uint32_t entry);

#endif