#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

#include "multitask/message.h"
#include "memory/pageFrame.h"
#include "kstdlib.h"

#define MAX_MESSAGES (PAGE_SIZE * 10 / sizeof(Message))

namespace Multitask
{
    enum class TaskType
    {
        KERNEL,
        USER
    };

    class Task
    {
        public:
            Task(char const* sName, const TaskType type, const uint32_t entrypoint);

            void SwitchToTask();
            void SwitchFromTask();
            void SetEntrypoint(const uint32_t entrypoint);

            void LoadFromTask(const Task& task);

            // Task data
            char m_sName[32];
            uint32_t* m_pStack;
            uint32_t m_Entrypoint;
            TaskType m_Type;
            uint32_t m_PID;
            bool m_bBlocked;
            bool m_bBlockedFilter;
            uint32_t m_blockedFilter;

            // Page frame - technically kernel tasks don't need one but hey-ho
            Memory::PageFrame m_PageFrame;

            // Sbrk implementation
            void* m_pSbrkBuffer;
            uint32_t m_nSbrkBytesUsed;
            
            // Message queue
            uint32_t m_nMessages = 0;

            void AddMesage(const uint32_t sourcePID, uint8_t* pData);
            void GetMessage(Message* pMessage);
            void RemoveMessage(const uint32_t filter);
            void Block();
            void Block(const uint32_t filter);
            void Unblock();
            bool HasMessages() const;
            bool HasMessage(const uint32_t filter) const;
            
        private:
            // Message queue
            Message* m_messages;
    };

    void Init();
    int CreateTask(char const* sName);
    int CreateTask(char const* sName, const TaskType type, void (*entrypoint)());
    
    Task* GetCurrentTask();
    void RemoveCurrentTask();
    void RemoveTaskWithID(const uint32_t pid);
    Task* GetTaskWithID(const uint32_t pid);

    extern uint32_t nTasks;

    extern "C"
    {
        // Variables for assembly
        extern uint32_t* pSavedTaskStack;
        extern uint32_t* pNewTaskStack;
        extern volatile bool bPrivilegeChange;

        void OnTaskSwitch(const bool bPIT);
        void IRQ0();
    }
}

#endif
