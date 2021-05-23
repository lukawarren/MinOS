#pragma once
#ifndef FILE_h
#define FILE_h

#include <stdint.h>
#include <stddef.h>

#include "kstdlib.h"

namespace Filesystem
{
    
    class File
    {
    public:
        constexpr bool IsFile() const;
        constexpr bool IsDevice() const;

        void Write(void* data)
        {
            memcpy(m_pData, data, m_Size);
        } 

        void* m_pData = nullptr;
        uint32_t m_Size = 0;
    };

}

#endif