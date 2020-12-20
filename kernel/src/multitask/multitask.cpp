#include "multitask.h"
#include "../interrupts/interrupts.h"
#include "../memory/paging.h"
#include "../gfx/vga.h"
#include "stdlib.h"
#include "taskSwitch.h"

bool bEnableMultitasking = false;

// Linked list of tasks
Task* pTaskListHead = nullptr;
Task* pTaskListTail = nullptr;
Task* pCurrentTask = nullptr;
size_t nTasks = 0;

// Ring 0 vs Ring 3
TSS* tss = nullptr;
extern uint32_t __tss_stack;
static uint32_t lastUserTaskPages = 0;

Task* CreateTask(char const* sName, uint32_t entry, uint32_t size, uint32_t location, TaskType type)
{
    // Create new task in memory and linked list
    Task* task = (Task*) kmalloc(sizeof(Task), type == KERNEL_TASK ? KERNEL_PAGE : USER_PAGE);
    strncpy(task->sName, sName, 32);
    task->bKernel = type == KERNEL_PAGE;

    // Round task to nearest page
    uint32_t originalSize = size;
    uint32_t roundedSize = originalSize;
    uint32_t remainder = roundedSize % PAGE_SIZE;
    if (remainder != 0) roundedSize += PAGE_SIZE - remainder;
    task->size = roundedSize;
    task->location = location;

    // Allocate stack
    uint32_t stack = (uint32_t)kmalloc(4096, type == KERNEL_TASK ? KERNEL_PAGE : USER_PAGE, type == KERNEL_TASK);
    task->pStack = (uint32_t*)(stack + 4096 - 16); // Stack grows downwards
    uint32_t* pStackTop = task->pStack;
    task->pOriginalStack = (uint32_t*)stack;
    
    // Get eflags
    uint32_t eflags;
    asm volatile( "pushf; pop %0;" : "=rm"(eflags) );
  
    // Push blank registers onto the stack
    *--task->pStack = type == KERNEL_TASK ? 0x10 : 0x23;   // stack segment (ss)
    *--task->pStack = (uint32_t) pStackTop; // esp
    *--task->pStack = eflags; // eflags
    *--task->pStack = type == KERNEL_TASK ? 0x08 : 0x1B;    // cs (iret uses a 32-bit pop - don't panic!)
    *--task->pStack = entry;  // eip
    *--task->pStack = 0;      // eax
    *--task->pStack = 0;      // ebx
    *--task->pStack = 0;      // ecx
    *--task->pStack = 0;      // edx
    *--task->pStack = 0;      // esi
    *--task->pStack = 0;      // edi
    *--task->pStack = (uint32_t) pStackTop; // ebp?
    *--task->pStack = eflags; // eflags (yes, twice)

    // Segment registers
    *--task->pStack = type == KERNEL_TASK ? 0x10 : 0x23; // ds
    *--task->pStack = type == KERNEL_TASK ? 0x10 : 0x23; // fs
    *--task->pStack = type == KERNEL_TASK ? 0x10 : 0x23; // es
    *--task->pStack = type == KERNEL_TASK ? 0x10 : 0x23; // gs

    // Linked list stuff
    Task* oldHead = pTaskListHead;
    pTaskListHead = task;
    task->pPrevTask = oldHead;
    oldHead->pNextTask = task;
    
    if (pTaskListTail == nullptr) pTaskListTail = task;
    if (task->pNextTask == nullptr) task->pNextTask = pTaskListTail;

    nTasks++;

    return task;
}

void SetTSSForMultitasking(TSS* _tss) { tss = _tss; }

void EnableScheduler()              { bEnableMultitasking = true; }
void DisableScheduler()             { bEnableMultitasking = false; }

static void MapNewUserTask(Task* task)
{
    // Unmap current task so its memory can't be read or written to accidentally
    for (uint32_t i = 0; i < lastUserTaskPages; ++i)
    {
        DeallocatePage(0x40000000 + i*PAGE_SIZE);
    } 

    // Setup paging so task begins at 0x40000000
    for (uint32_t i = 0; i < task->size / PAGE_SIZE; ++i)
    {
        AllocatePage(task->location + i * PAGE_SIZE, 0x40000000 + i * PAGE_SIZE, USER_PAGE, false);
    }

    lastUserTaskPages = task->size / PAGE_SIZE;
}

void OnMultitaskPIT()
{
    if (nTasks == 0 || !bEnableMultitasking) { bIRQShouldJump = false; return; }

    // If one task, switch to it if nessecary
    if (nTasks == 1 && pCurrentTask == nullptr) 
    {
        VGA_printf("Switching...");
        pCurrentTask = pTaskListHead;
        oldTaskStack = 0;
        newTaskStack = (uint32_t) &pCurrentTask->pStack;
        if (pCurrentTask->bKernel == false) MapNewUserTask(pCurrentTask);
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
            if (newTask->bKernel == false) MapNewUserTask(newTask);
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
            if (newTask->bKernel == false) MapNewUserTask(newTask);
            bIRQShouldJump = true; // Will tell the following IRQ 0 to switch tasks
        }      
    }
}

uint32_t GetNumberOfTasks()
{
    return nTasks;
}

void TaskExit()
{
    Task* task = pCurrentTask;

    /*
    // Update linked list
    if (task->pPrevTask != nullptr) 
    {
        pCurrentTask = task->pPrevTask;
        pCurrentTask->pNextTask = task->pNextTask;
        if (pCurrentTask->pNextTask != pTaskListTail) pCurrentTask->pNextTask->pPrevTask = pCurrentTask;   
    }   
    if (task->pNextTask != pTaskListHead)
    {
        pCurrentTask = task->pNextTask;
    } 
    else pCurrentTask = nullptr;
    */

    /*
    if (pTaskListHead == task && nTasks > 1) pTaskListHead = task->pPrevTask;
    if (pTaskListTail == task && nTasks > 1) pTaskListTail = task->pNextTask;

    if (task->pNextTask != task && nTasks > 2)
    {
        pCurrentTask = task->pPrevTask;
        pCurrentTask->pNextTask = task->pNextTask;
    }
    else pCurrentTask = nullptr;
    */

    pCurrentTask = task->pPrevTask;
    pCurrentTask->pNextTask = task->pNextTask;
    pTaskListHead = pCurrentTask;

    // Unallocate all memory
    kfree(task->pOriginalStack, 4096); // stack
    if (task->size != 0) kfree((void*)task->location, task->size); // memory
    kfree(task, sizeof(Task)); // task struct

    // Switch to new task
    nTasks--;
    OnMultitaskPIT();
}