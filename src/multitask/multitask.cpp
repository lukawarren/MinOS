#include "multitask.h"
#include "../memory/paging.h"
#include "../gfx/vga.h"
#include "../stdlib.h"

// Linked list of tasks
Task* pTaskListHead = nullptr;

Task* CreateTask(char const* sName, uint32_t entry)
{
    // Create new task in memory and linked list
    Task* task = (Task*) kmalloc(sizeof(Task));
    strncpy(task->sName, sName, 32);

    // Allocate stack
    task->pStack = (uint32_t*)((uint32_t)kmalloc(4096) + 4096); // Stack grows downwards
    uint32_t* pStackTop = task->pStack;

    // Push blank registers onto the stack
    // No need for esp, that's just the stack
    *--task->pStack = entry;  // eip
    *--task->pStack = 0;      // eax
    *--task->pStack = 0;      // ebx
    *--task->pStack = 0;      // ecx
    *--task->pStack = 0;      // edx
    *--task->pStack = 0;      // esi
    *--task->pStack = 0;      // edi
    *--task->pStack = (uint32_t) pStackTop; // ebp?

    // Linked list stuff
    Task* oldHead = pTaskListHead;
    pTaskListHead = task;
    task->pPrevTask = oldHead;
    oldHead->pNextTask = task;

    VGA_printf("[Info] ", false, VGA_COLOUR_LIGHT_YELLOW);
    VGA_printf("Created new task - ", false);
    VGA_printf(task->sName);

    return task;
}