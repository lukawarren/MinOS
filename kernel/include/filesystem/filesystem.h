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

        userspace = 5,

        N_FILES = 6
    };

    void Init();
    File* GetFile(const FileDescriptor fd);
}

#endif