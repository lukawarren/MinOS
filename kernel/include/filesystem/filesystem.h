#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stddef.h>

#include "filesystem/file.h"

namespace Filesystem
{
    
    typedef uint32_t FileDescriptor;

    enum FileDescriptors
    {
        stdin = 0,
        stdout = 1,
        stderr = 2,

        framebuffer = 3,
        mouse = 4,

        wm = 5,
        getpagesize = 6,

        N_FILES = 7
    };

    void Init();
    File* GetFile(const FileDescriptor fd);
    File* GetFile(char const* sName);
}

#endif