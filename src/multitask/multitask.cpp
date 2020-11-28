#include "multitask.h"
#include "../interrupts/interrupts.h"
#include "../memory/paging.h"
#include "../gfx/vga.h"
#include "../stdlib.h"

bool bEnableMultitasking = false;

// Linked list of tasks
Task* pTaskListHead = nullptr;
Task* pCurrentTask = nullptr;
size_t nTasks = 0;

Task* oldTask;
Task* newTask;

uint32_t IRQReturnAddress = (uint32_t) &OnIRQReturn;

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

    nTasks++;

    VGA_printf("[Info] ", false, VGA_COLOUR_LIGHT_YELLOW);
    VGA_printf("Created new task - ", false);
    VGA_printf(task->sName);

    return task;
}

void EnableScheduler()  { bEnableMultitasking = true;  }
void DisableScheduler() { bEnableMultitasking = false; }

void OnMultitaskPIT()
{
    /*
        So.... two processes breaks it.
        But why? Even with nothing below it still breaks - *but check that*.
        So.... something that CreateTask does?
    */

    if (nTasks == 0 || !bEnableMultitasking) { bIRQShouldJump = false; return; }

    // If one task, switch to it if nessecary
    if (nTasks == 1 && pCurrentTask == nullptr) 
    {
        pCurrentTask = pTaskListHead;
        oldTask = pCurrentTask;
        newTask = pCurrentTask;

        bIRQShouldJump = true;
    }
    else if (nTasks > 1)
    {
        /*
        if (pCurrentTask == nullptr) pCurrentTask = pTaskListHead;
        
        oldTask = pCurrentTask;
        newTask = pCurrentTask->pNextTask;

        // If end of list reached, go back to start
        if (newTask == nullptr)
        {
            while (newTask == nullptr || newTask->pPrevTask != nullptr) newTask = oldTask->pPrevTask;
        }

        pCurrentTask = newTask;*/

        pCurrentTask = pTaskListHead->pPrevTask;
        oldTask = pCurrentTask;
        newTask = pCurrentTask;
        bIRQShouldJump = true;
    }
}

void OnIRQReturn()
{
    VGA_printf<uint32_t, true>((uint32_t)pCurrentTask);
    while (true) { asm("nop"); }
    SwitchToTask((uint32_t)&oldTask->pStack, (uint32_t)newTask->pStack);
}