#include "multitask/multitask.h"
#include "multitask/elf.h"
#include "memory/memory.h"
#include "memory/modules.h"
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

    Task::Task(char const* sName, const TaskType type, const uint32_t entrypoint)
    {
        // Set member variables
        strncpy(m_sName, sName, sizeof(m_sName));
        m_Type = type;
        m_Entrypoint = entrypoint;
        
        // Create stack - 128kb, 32 pages - that grows downwards - minus at least 1 to not go over 1 page, but actually 16 to ensure alignment
        const uint32_t stackSize = PAGE_SIZE * 32;
        const uint32_t stackBeginInMemory = (uint32_t)(Memory::kPageFrame.AllocateMemory(stackSize, KERNEL_PAGE));
        m_pStack = (uint32_t*) (stackBeginInMemory + stackSize-16);
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
        *--m_pStack = m_Entrypoint;         // eip

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

        // Setup page frame allocator for userspace processes
        if (type == TaskType::USER)
            m_PageFrame = Memory::PageFrame(stackBeginInMemory, stackSize);

        // CR3 on stack as if we swtitched to it in C code,
        // whilst using another task's stack, things'd get funky
        if (type == TaskType::USER) *--m_pStack = m_PageFrame.GetCR3();
        else  *--m_pStack = Memory::kPageFrame.GetCR3();
    }

    void Task::SwitchToTask()
    {
        pNewTaskStack = (uint32_t*) &(m_pStack);
    }

    void Task::SwitchFromTask()
    {
        pSavedTaskStack = (uint32_t*) &(m_pStack);
    }

    void Task::SetEntrypoint(const uint32_t entrypoint)
    {
        // Modify entrypoint in stack
        m_pStack += 14;
        *--m_pStack = entrypoint;
        m_pStack -= 13;
    }

    void Task::LoadFromTask(const Task& task)
    {
        // Glorified copy constructor
        strncpy(m_sName, task.m_sName, sizeof(m_sName) / sizeof(m_sName[0]));
        m_pStack = task.m_pStack;
        m_Entrypoint = task.m_Entrypoint;
        m_Type = task.m_Type;
        m_PageFrame = task.m_PageFrame;
    }

    void Init()
    {
        // Create malloc "slab"
        tasks = (Task*) Memory::kPageFrame.AllocateMemory(sizeof(Task) * maxTasks, KERNEL_PAGE);
    }

    int CreateTask(char const* sName)
    {
        // Check we have room
        assert(nTasks < maxTasks);
        if (nTasks >= maxTasks) return -1;

        // Create task
        tasks[nTasks] = Task(sName, TaskType::USER, 0xdeadbeef);
    
        // Load ELF module into memory
        const uint32_t pModule = Modules::GetModule();
        const uint32_t entrypoint = Multitask::LoadElfProgram(pModule, tasks[nTasks].m_PageFrame);
        tasks[nTasks].SetEntrypoint(entrypoint);

        nTasks++;
        return nTasks-1;
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

    void OnTaskSwitch(const bool bPIT)
    {
        assert(nTasks > 0);

        // If we came from kernel, no need to save previous "task"
        if (bCameFromKernel)
        {
            bCameFromKernel = false;
            tasks[nCurrentTask].SwitchToTask();

            // Set the privilege change level for next time
            if (tasks[nCurrentTask].m_Type == TaskType::KERNEL) bPrivilegeChange = false;
            else bPrivilegeChange = true;
        }
        else
        {
            tasks[nPreviousTask].SwitchFromTask();
            tasks[nCurrentTask].SwitchToTask();

            // Set the privilege change level for next time
            if (tasks[nCurrentTask].m_Type == TaskType::KERNEL) bPrivilegeChange = false;
            else bPrivilegeChange = true;
        }

        // Save previous task then advance current taks by 1 (or loop back around)
        nPreviousTask = nCurrentTask;
        nCurrentTask++;
        if (nCurrentTask >= nTasks) nCurrentTask = 0;

        if (bPIT)
        {
            PIT::Reset(); // Must be reset every interrupt so as to fire again
            PIC::EndInterrupt(0x20); // Offset is 20, and it's IRQ 0
        }
    }

    Task& GetCurrentTask()
    {
        // See below comment
        return tasks[nPreviousTask];
    }

    void RemoveCurrentTask()
    {
        assert(nTasks > 1);

        // Deallocate all the task's memory (including stack) - see below for "nPreviousTask"
        tasks[nPreviousTask].m_PageFrame.FreeAllPages();

        // We're in 1 big array, so find the element, and shift all above elements downwards
        // (unless we're the last task in the array, in which case don't do anything).
        // Current task is actually the next task to be ran (I know, I know...),
        // hence the use of the previous task (I really have outdone myself)
        if (nCurrentTask != maxTasks-1)
        {
            for (uint32_t i = nPreviousTask; i < nTasks; ++i)
            {
                tasks[i].LoadFromTask(tasks[i+1]);
            }
        }
        nTasks--;

        // Reset to original, guaranteed state
        bCameFromKernel = true;
        nCurrentTask = 0;
        nPreviousTask = 0;
        pSavedTaskStack = 0;
    }

}