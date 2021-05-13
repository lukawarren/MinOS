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
            Task(char const* sName, const TaskType type, uint32_t entrypoint, uint32_t codeSize = 0);

            void SwitchToTask();
            void SwitchFromTask();

        private:
            char m_sName[32];
            uint32_t* m_pStack;
            uint32_t m_Entrypoint;
            TaskType m_Type;
            Memory::PageFrame m_PageFrame; // Technically kernel tasks don't need one but hey-ho
    };

    void Init();
    int CreateTask(char const* sName, const TaskType type, uint32_t entrypoint, uint32_t codeSize);
    int CreateTask(char const* sName, const TaskType type, void (*entrypoint)());

    extern "C"
    {
        // Variables for assembly
        extern uint32_t* pSavedTaskStack;
        extern uint32_t* pNewTaskStack;

        void OnPIT();
        void IRQ0();
    }
}

#endif