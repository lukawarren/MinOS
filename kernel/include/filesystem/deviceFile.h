#pragma once
#ifndef DEVICE_FILE_H
#define DEVICE_FILE_H

#include <stdint.h>
#include <stddef.h>

#include "filesystem/file.h"
#include "io/framebuffer.h"
#include "kstdlib.h"

namespace Filesystem
{
    
    class DeviceFile : public File
    {
    public:
        DeviceFile(const uint32_t size, void* pData, const char* sName, uint32_t iNode)
        {
            m_Size = size;
            m_pData = pData;
            strncpy(m_sName, sName, sizeof(m_sName));
            m_iNode = iNode;
        }

        constexpr bool IsFile() const { return false; }
        constexpr bool IsDevice() const { return true; }

        void Write(void* data);
    };

}

#endif