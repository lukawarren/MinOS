#pragma once
#ifndef FRAMEBUFFER_FILE_H
#define FRAMEBUFFER_FILE_H

#include <stdint.h>
#include <stddef.h>

#include "filesystem/file.h"
#include "io/framebuffer.h"
#include "kstdlib.h"

namespace Filesystem
{
    
    class FramebufferFile : public File
    {
    public:
        FramebufferFile()
        {
            assert(Framebuffer::sFramebuffer.address != 0);
            m_size = Framebuffer::sFramebuffer.size;
            m_pData = (void*) Framebuffer::sFramebuffer.address;
        }

        constexpr bool IsFile() const { return false; }
        constexpr bool IsDevice() const { return true; }

        void Write(void* data);
    };

}

#endif