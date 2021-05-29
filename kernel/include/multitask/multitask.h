#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

#include "memory/pageFrame.h"

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

            // Paeg frame - technically kernel tasks don't need one but hey-ho
            Memory::PageFrame m_PageFrame;

            // Sbrk implementation
            void* m_pSbrkBuffer;
            uint32_t m_nSbrkBytesUsed;
    };

    void Init();
    int CreateTask(char const* sName);
    int CreateTask(char const* sName, const TaskType type, void (*entrypoint)());
    
    Task* GetCurrentTask();
    void RemoveCurrentTask();
    void RemoveTaskWithID(const uint32_t pid);

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