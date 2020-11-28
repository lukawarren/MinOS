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
    if (nTasks == 0 || !bEnableMultitasking) return;

    // If one task, switch to it if nessecary
    if (nTasks == 1 && pCurrentTask == nullptr) 
    {
        pCurrentTask = pTaskListHead;
        //PIC_EndInterrupt(currentIRQ);  // FINISH INTERRUPT (interrupts will not return otherwise)
        SwitchToTask((uint32_t)&pCurrentTask->pStack, (uint32_t)pCurrentTask->pStack);
    }
    else if (nTasks > 1)
    {
        if (pCurrentTask == nullptr) pCurrentTask = pTaskListHead;

        Task* oldTask = pCurrentTask;
        Task* newTask = pCurrentTask->pNextTask;

        // If end of list reached, go back to start
        if (newTask == nullptr)
        {
            while (newTask == nullptr || newTask->pPrevTask != nullptr) newTask = oldTask->pPrevTask;
        }

        pCurrentTask = newTask;

        //PIC_EndInterrupt(currentIRQ);  // FINISH INTERRUPT (interrupts will not return otherwise)
        SwitchToTask((uint32_t)&oldTask->pStack, (uint32_t)newTask->pStack);
    }
}
