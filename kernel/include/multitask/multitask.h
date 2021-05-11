#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

namespace Multitask
{
    class Task
    {
        public:
            Task(char const* sName, uint32_t entrypoint);

            void SwitchToTask();
            void SwitchFromTask();

        private:
            char m_sName[32];
            uint32_t* m_pStack;
            uint32_t m_Entrypoint;
    };

    void Init();
    int CreateTask(char const* sName, void (*entrypoint)());

    extern "C"
    {
        // Variables for assembly
        extern uint32_t* pSavedTaskStack;
        extern uint32_t newTaskStack;

        void OnPIT();
        void IRQ0();
    }
}

#endif