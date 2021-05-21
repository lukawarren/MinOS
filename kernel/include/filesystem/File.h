#pragma once
#ifndef FILE_h
#define FILE_h

#include <stdint.h>
#include <stddef.h>

namespace Filesystem
{
    
    class File
    {
    public:
        File();

        constexpr bool IsFile() const;
        constexpr bool IsDevice() const;

    };

}

#endif