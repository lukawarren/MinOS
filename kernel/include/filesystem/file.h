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

        File() {}

        File(const uint32_t size, void* pData, char const* sName, uint32_t iNode)
        {
            m_Size = size;
            m_pData = pData;
            strncpy(m_sName, sName, sizeof(m_sName));
            m_iNode= iNode;
        }

        constexpr bool IsFile() const;
        constexpr bool IsDevice() const;

        void Write(void* data)
        {
            memcpy(m_pData, data, m_Size);
        } 

        void* m_pData = nullptr;
        uint32_t m_Size = 0;
        char m_sName[128];
        uint32_t m_iNode = 0;
    };

}

#endif