#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

#include "../memory/tss.h"

struct Task
{
    char sName[32];
    bool bKernel;
    uint32_t size;
    uint32_t location;
    uint32_t* pStack;
    Task* pPrevTask = nullptr;
    Task* pNextTask = nullptr;
};

void SetTSSForMultitasking(TSS* _tss);

void EnableScheduler();
void DisableScheduler();

void OnMultitaskPIT();

enum TaskType
{
    KERNEL_TASK,
    USER_TASK
};

Task* CreateTask(char const* sName, uint32_t entry, uint32_t size = 0, uint32_t location = 0, TaskType type = KERNEL_TASK);

#endif