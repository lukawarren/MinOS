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
            Task(char const* sName, uint32_t entrypoint, uint32_t* pStack);
        
        private:
            char m_sName[32];
            uint32_t* m_pStack;
            uint32_t m_Entrypoint;
    };

    void Init();
    int CreateTask(char const* sName, const uint32_t entrypoint);
}

#endif