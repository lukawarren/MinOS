#include "multitask.h"
#include "../interrupts/interrupts.h"
#include "../memory/paging.h"
#include "../gfx/vga.h"
#include "../stdlib.h"
#include "taskSwitch.h"

bool bEnableMultitasking = false;

// Linked list of tasks
Task* pTaskListHead = nullptr;
Task* pTaskListTail = nullptr;
Task* pCurrentTask = nullptr;
size_t nTasks = 0;

Task* CreateTask(char const* sName, uint32_t entry)
{
    // Create new task in memory and linked list
    Task* task = (Task*) kmalloc(sizeof(Task));
    strncpy(task->sName, sName, 32);

    // Allocate stack
    task->pStack = (uint32_t*)((uint32_t)kmalloc(4096) + 4096 - 0x10); // Stack grows downwards
    uint32_t* pStackTop = task->pStack;
    
    // Get eflags
    uint32_t eflags;
    asm volatile( "pushf; pop %0;" : "=rm"(eflags) );
  
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
    *--task->pStack = eflags;
    
    // Linked list stuff
    Task* oldHead = pTaskListHead;
    pTaskListHead = task;
    task->pPrevTask = oldHead;
    oldHead->pNextTask = task;
    
    if (pTaskListTail == nullptr) pTaskListTail = task;
    if (task->pNextTask == nullptr) task->pNextTask = pTaskListTail;

    nTasks++;

    VGA_printf("[Info] ", false, VGA_COLOUR_LIGHT_YELLOW);
    VGA_printf("Created new task - ", false);
    VGA_printf(task->sName, false);
    VGA_printf(" - entrypoint ", false);
    VGA_printf<uint32_t, true>((uint32_t)entry, false);
    VGA_printf(", stack - ", false);
    VGA_printf<uint32_t, true>((uint32_t)pStackTop);

    return task;
}

void EnableScheduler()  { bEnableMultitasking = true; }
void DisableScheduler() { bEnableMultitasking = false; }

void OnMultitaskPIT()
{
    if (nTasks == 0 || !bEnableMultitasking) { bIRQShouldJump = false; return; }

    // If one task, switch to it if nessecary
    if (nTasks == 1 && pCurrentTask == nullptr) 
    {
        pCurrentTask = pTaskListHead;
        oldTaskStack = 0;
        newTaskStack = (uint32_t) &pCurrentTask->pStack;
        bIRQShouldJump = true; // Will tell the following IRQ 0 to switch tasks
    }
    else if (nTasks > 1)
    {
        // If no task has previously ran, avoid
        // sullying the non-existent "old task"
        if (pCurrentTask == nullptr)
        {
            pCurrentTask = pTaskListTail;
            Task* newTask = pCurrentTask;
            oldTaskStack = 0;
            newTaskStack = (uint32_t) &newTask->pStack;
            bIRQShouldJump = true; // Will tell the following IRQ 0 to switch tasks
        }

        // Otherwise continue cycling
        else
        {
            Task* oldTask = pCurrentTask;
            Task* newTask = pCurrentTask->pNextTask;

            pCurrentTask = newTask;
            oldTaskStack = (uint32_t) &oldTask->pStack;
            newTaskStack = (uint32_t) &newTask->pStack;
            bIRQShouldJump = true; // Will tell the following IRQ 0 to switch tasks
        }      
    }
}
