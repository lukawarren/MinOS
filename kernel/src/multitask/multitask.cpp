#include "multitask/multitask.h"
#include "memory/memory.h"
#include "stdlib.h"

namespace Multitask
{

    // Task malloc "slab" that neatly fills two pages
    constexpr uint32_t maxTasks = PAGE_SIZE / sizeof(Task);
    static uint32_t nTasks;
    static Task* tasks;

    Task::Task(char const* sName, uint32_t entrypoint)
    {
        // Set member variables
        strncpy(m_sName, sName, sizeof(m_sName));
        m_Entrypoint = entrypoint;
        
        // Create stack - 128kb, 32 pages - that grows downwards - minus at least 1 to not go over 1 page, but actually 16 to ensure alignment
        m_pStack = (uint32_t*) ((uint32_t)(Memory::AllocateMemory(PAGE_SIZE * 32)) + PAGE_SIZE*32-1);
        const uint32_t stackTop = (uint32_t) m_pStack;

        // Setup stack at point of iret
        *--m_pStack = 0x00;                 // stack alignment (if any)
        *--m_pStack = 0x23;                 // stack segment (ss)
        *--m_pStack = stackTop;             // esp
        *--m_pStack = 0x202;                // eflags - default value with interrupts enabled
        *--m_pStack = 0x1B;                 // cs
        *--m_pStack = entrypoint;           // eip
        *--m_pStack = 0;                    // eax
        *--m_pStack = 0;                    // ebx
        *--m_pStack = 0;                    // ecx
        *--m_pStack = 0;                    // edx
        *--m_pStack = 0;                    // esi
        *--m_pStack = 0;                    // edi
        *--m_pStack = stackTop;             // ebp
    }

    void Init()
    {
        // Create malloc "slab"
        tasks = (Task*) Memory::AllocateMemory(sizeof(Task) * maxTasks);
    }

    int CreateTask(char const* sName, void (*entrypoint)())
    {
        // Check we have room
        if (nTasks >= maxTasks) return -1;

        // Create task and return index
        tasks[nTasks] = Task(sName, (uint32_t)entrypoint);

        nTasks++;
        return nTasks-1;
    }

}