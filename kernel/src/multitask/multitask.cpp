#include "multitask/multitask.h"
#include "multitask/elf.h"
#include "filesystem/filesystem.h"
#include "memory/memory.h"
#include "memory/modules.h"
#include "cpu/pit.h"
#include "cpu/pic.h"
#include "io/gfx/framebuffer.h"
#include "io/uart.h"
#include "kstdlib.h"

namespace Multitask
{
    // Task malloc "slab" that neatly fills two pages
    constexpr uint32_t maxTasks = PAGE_SIZE * 32 / sizeof(Task);
    static Task* tasks;

    // House keeping
    uint32_t nTasks = 0;
    static uint32_t nCurrentTask = 0;
    static uint32_t nPreviousTask = 0;
    static uint32_t nPIDs = 0;

    // If we've coming from the kernel, there is no need to save our state
    static bool bCameFromKernel = true;

    Task::Task(char const* sName, const TaskType type, const uint32_t entrypoint)
    {
        // Set member variables
        strncpy(m_sName, sName, sizeof(m_sName));
        m_Type = type;
        m_Entrypoint = entrypoint;
        m_PID = ++nPIDs;
        m_bBlocked = false;
        m_nMicroseconds = 0;

        // Create stack - 128kb, 32 pages - that grows downwards - minus at least 1 to not go over 1 page, but actually 20 to ensure alignment
        const uint32_t stackSize = PAGE_SIZE * 32;
        const uint32_t stackBeginInMemory = (uint32_t)(Memory::kPageFrame.AllocateMemory(stackSize, KERNEL_PAGE)); // Mapped as user by *it's own* page frame
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
        {
            m_PageFrame = Memory::PageFrame(stackBeginInMemory, stackSize);
            m_pSbrkBuffer = nullptr;
            m_nSbrkBytesUsed = 0;
            m_messages = (Message*) m_PageFrame.AllocateMemory(MAX_MESSAGES * sizeof(Message), KERNEL_PAGE);
        }

        // CR3 on stack as if we swtitched to it in C code,
        // whilst using another task's stack, things'd get funky
        if (type == TaskType::USER) *--m_pStack = m_PageFrame.GetCR3();
        else  *--m_pStack = Memory::kPageFrame.GetCR3();
        
        UART::WriteString("[Multitask] Task ");
        UART::WriteString(sName);
        UART::WriteString(" created with PID ");
        UART::WriteNumber(m_PID);
        UART::WriteString("\n");
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
        m_nSbrkBytesUsed = task.m_nSbrkBytesUsed;
        m_pSbrkBuffer = task.m_pSbrkBuffer;
        memcpy(m_messages, task.m_messages, sizeof(m_messages));
        m_PID = task.m_PID;
        m_bBlocked = task.m_bBlocked;
        m_bBlockedFilter = task.m_bBlockedFilter;
        m_blockedFilter = task.m_blockedFilter;
        m_nMessages = task.m_nMessages;
        m_nMicroseconds = task.m_nMicroseconds;
    }

    void Task::AddMesage(const uint32_t sourcePID, uint8_t* pData)
    {
        if (m_nMessages > MAX_MESSAGES)
        {
            assert(false);
            return;
        }
        
        // Add message
        auto message = &m_messages[m_nMessages];
        message->sourcePID = sourcePID;
        memcpy(message->data, pData, sizeof(message->data));
        m_nMessages++;
        
        // Unblock if need be
        if (m_bBlockedFilter == false) m_bBlocked = false;
        else if (HasMessage(m_blockedFilter))
        {
            m_bBlocked = false;
            m_bBlockedFilter = false;
        }
    }
    
    void Task::GetMessage(Message* pMessage)
    {
        assert(m_nMessages > 0);
        
        // Copy to destination
        memcpy(pMessage, &m_messages[0], sizeof(Message));
        
        // Shift all messages down the line
        for (uint32_t i = 0; i < m_nMessages; ++i)
            memcpy(&m_messages[i], &m_messages[i+1], sizeof(Message));
        
        m_nMessages--;
    }
    
    void Task::RemoveMessage(uint32_t filter)
    {
        assert(m_nMessages > 0);
        
        const uint8_t bytes[4] =
        {
            uint8_t(filter >> 24),
            uint8_t(filter >> 16),
            uint8_t(filter >> 8),
            uint8_t(filter)
        };
        
        // Find first event satisfying first four bytes and remove it
        for (uint32_t i = 0; i < m_nMessages; ++i)
        {
            if (m_messages[i].data[0] == bytes[0] && m_messages[i].data[1] == bytes[1] &&
                m_messages[i].data[2] == bytes[2] && m_messages[i].data[3] == bytes[3])
            {
                // Shift events down by one
                for (uint32_t j = i; j < m_nMessages; ++j)
                {
                    memcpy(&m_messages[j], &m_messages[j+1], sizeof(Message));
                }
                
                m_nMessages--;
                return;
            }
        }
        
        assert(false);
        while(1) {}
    }
    
    void Task::Block()
    {
        m_bBlocked = true;
        m_bBlockedFilter = false;
        OnTaskSwitch(false);
    }
    
    void Task::Block(const uint32_t filter)
    {
        m_bBlocked = true;
        m_bBlockedFilter = true;
        m_blockedFilter = filter;
        OnTaskSwitch(false);
    }
    
    void Task::Unblock()
    {
        m_bBlocked = false;
        OnTaskSwitch(false);
    }
    
    bool Task::HasMessages() const
    {
        return m_nMessages > 0;
    }
    
    bool Task::HasMessage(const uint32_t filter) const
    {
        const uint8_t bytes[4] =
        {
            uint8_t(filter >> 24),
            uint8_t(filter >> 16),
            uint8_t(filter >> 8),
            uint8_t(filter)
        };
        
        // Find first event satisfying first four bytes
        for (uint32_t i = 0; i < m_nMessages; ++i)
            if (m_messages[i].data[0] == bytes[0] && m_messages[i].data[1] == bytes[1] &&
                m_messages[i].data[2] == bytes[2] && m_messages[i].data[3] == bytes[3])
                return true;
        
        return false;
    }

    void Task::SleepForMicroseconds(const uint32_t microseconds)
    {
        m_nMicroseconds = microseconds;
        OnTaskSwitch(false);
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
        const uint32_t entrypoint = Multitask::LoadElfProgram
        (
            (uint32_t)Filesystem::GetFile(sName)->m_pData,
            tasks[nTasks].m_PageFrame
        );
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

        if (bPIT)
        {
            // Decrease sleeping tasks' duration by a length equal to the PIT's delay
            const auto delay = PIT::GetDelayInMicroseconds();
            for (uint32_t i = 0; i < nTasks; ++i)
            {
                // Avoid underflows
                if (tasks[i].m_nMicroseconds >= delay) tasks[i].m_nMicroseconds -= delay;
                else tasks[i].m_nMicroseconds = 0;
            }
        }

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
            
            // Avoid blocked tasks
            while (nTasks > 1 && (tasks[nCurrentTask].m_bBlocked || tasks[nCurrentTask].m_nMicroseconds))
            {
                nCurrentTask++;
                if (nCurrentTask >= nTasks) nCurrentTask = 0;
            }
            
            tasks[nCurrentTask].SwitchToTask();

            // Set the privilege change level for next time
            if (tasks[nCurrentTask].m_Type == TaskType::KERNEL) bPrivilegeChange = false;
            else bPrivilegeChange = true;
        }

        // Save previous task then advance current task by 1 (or loop back around)
        nPreviousTask = nCurrentTask;
        nCurrentTask++;
        if (nCurrentTask >= nTasks) nCurrentTask = 0;

        if (bPIT)
        {
            PIT::Reset(); // Must be reset every interrupt so as to fire again
            PIC::EndInterrupt(0x20); // Offset is 20, and it's IRQ 0
        }
    }

    Task* GetCurrentTask()
    {
        // See below comment
        return &tasks[nPreviousTask];
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

    void RemoveTaskWithID(const uint32_t pid)
    {
        if (pid == GetCurrentTask()->m_PID) RemoveCurrentTask();
        else assert(false); // (not implemented yet, or ever really)
    }

    Task* GetTaskWithID(const uint32_t pid)
    {
        for (uint32_t i = 0; i < nTasks; ++i)
        {
            if (tasks[i].m_PID == pid) return &tasks[i];
        }

        return nullptr;
    }

}
