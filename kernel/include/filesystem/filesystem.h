#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stddef.h>

#include "filesystem/file.h"

namespace Filesystem
{
    
    typedef uint32_t FileDescriptor;

    void Init();
    File GetFile(const FileDescriptor fd);

}

#endif