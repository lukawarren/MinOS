#include "multitask/multitask.h"
#include "memory/memory.h"
#include "cpu/pit.h"
#include "cpu/pic.h"
#include "stdlib.h"

namespace Multitask
{
    // Task malloc "slab" that neatly fills two pages
    constexpr uint32_t maxTasks = PAGE_SIZE * 2 / sizeof(Task);
    static Task* tasks;

    // House keepipng
    static uint32_t nTasks = 0;
    static uint32_t nCurrentTask = 0;
    static uint32_t nPreviousTask = 0;

    // If we've coming from the kernel, there is no need to save our state
    static bool bCameFromKernel = true;

    Task::Task(char const* sName, const TaskType type, uint32_t entrypoint)
    {
        // Set member variables
        strncpy(m_sName, sName, sizeof(m_sName));
        m_Entrypoint = entrypoint;
        m_Type = type;
        
        // Create stack - 128kb, 32 pages - that grows downwards - minus at least 1 to not go over 1 page, but actually 16 to ensure alignment
        m_pStack = (uint32_t*) ((uint32_t)(Memory::AllocateMemory(PAGE_SIZE * 32)) + PAGE_SIZE*32-1);
        const uint32_t stackTop = (uint32_t) m_pStack;

        // Chose our segment registers *carefully* depending on privilege level
        const uint32_t dataSegment = type == TaskType::KERNEL ? 0x10 : 0x23;
        const uint32_t codeSegment = type == TaskType::KERNEL ? 0x08 : 0x1B;

        // Setup stack for iret return
        *--m_pStack = 0x00;                 // stack alignment (if any)
        *--m_pStack = dataSegment;          // stack segment (ss)
        *--m_pStack = stackTop;             // esp
        *--m_pStack = 0x202;                // eflags - default value with interrupts enabled
        *--m_pStack = codeSegment;          // cs
        *--m_pStack = entrypoint;           // eip

        // Rest of stack, for restoring registers
        *--m_pStack = 0;                    // eax
        *--m_pStack = 0;                    // ecx
        *--m_pStack = 0;                    // edx
        *--m_pStack = 0;                    // ebx
        *--m_pStack = 0;                    // esp
        *--m_pStack = stackTop;             // ebp
        *--m_pStack = 0;                    // esi
        *--m_pStack = 0;                    // edi

        // Segment registers
        *--m_pStack = dataSegment; // ds
        *--m_pStack = dataSegment; // fs
        *--m_pStack = dataSegment; // es
        *--m_pStack = dataSegment; // gs
    }

    void Task::SwitchToTask()
    {
        pNewTaskStack = (uint32_t*) &(m_pStack);
    }

    void Task::SwitchFromTask()
    {
        pSavedTaskStack = (uint32_t*) &(m_pStack);
    }

    void Init()
    {
        // Create malloc "slab"
        tasks = (Task*) Memory::AllocateMemory(sizeof(Task) * maxTasks);
    }

    int CreateTask(char const* sName, const TaskType type, void (*entrypoint)())
    {
        // Check we have room
        assert(nTasks < maxTasks);
        if (nTasks >= maxTasks) return -1;

        // Create task and return index
        tasks[nTasks] = Task(sName, type, (uint32_t)entrypoint);

        nTasks++;
        return nTasks-1;
    }

    void OnPIT()
    {
        assert(nTasks > 0);

        // If we came from kernel, no need to save previous "task"
        if (bCameFromKernel)
        {
            bCameFromKernel = false;
            tasks[nCurrentTask].SwitchToTask();
        }
        else
        {
            tasks[nPreviousTask].SwitchFromTask();
            tasks[nCurrentTask].SwitchToTask();
        }

        // Save previous task then advance current taks by 1 (or loop back around)
        nPreviousTask = nCurrentTask;
        nCurrentTask++;
        if (nCurrentTask >= nTasks) nCurrentTask = 0;

        PIT::Reset(); // Must be reset every interrupt so as to fire again
        PIC::EndInterrupt(0x20); // Offset is 20, and it's IRQ 0
    }

}