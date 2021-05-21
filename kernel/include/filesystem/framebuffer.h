#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

#include "filesystem/file.h"

namespace Filesystem
{
    
    class FramebufferFile : public File
    {
    public:
        FramebufferFile();

        constexpr bool IsFile() const { return false; }
        constexpr bool IsDevice() const { return true; }

    };

}

#endif