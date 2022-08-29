#pragma once
#include "fs/file.h"
#include "memory/multiboot_info.h"

namespace fs
{
    typedef int descriptor;

    void init(const memory::MultibootInfo& info);
    Optional<DeviceFile*> get_file(const descriptor fd);

    // Temporary "devices"
    extern char keyboard_buffer[256];
    extern size_t keyboard_buffer_index;
    extern size_t wad_size;
    extern size_t wad_seek;
}