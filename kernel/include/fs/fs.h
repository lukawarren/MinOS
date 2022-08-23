#pragma once
#include "fs/file.h"

namespace fs
{
    typedef int descriptor;

    void init();
    Optional<DeviceFile*> get_file(const descriptor fd);
    extern char keyboard_buffer[256];
    extern size_t keyboard_buffer_index;
}