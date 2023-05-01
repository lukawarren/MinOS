#pragma once
#include "memory/multiboot_info.h"
#include "fs/device_fs.h"
#include "fs/types.h"

namespace fs
{
    void init(const memory::MultibootInfo& info, void(*register_devices)(DeviceFileSystem&));
    Optional<uint64_t> write(FileHandle handle, void* data, uint64_t offset, uint64_t length);
    Optional<uint64_t> read(FileHandle handle, void* data, uint64_t offset, uint64_t length);
    Optional<uint64_t> get_size(FileHandle handle);
    Optional<FileHandle> get_file(const char* path);

    constexpr FileHandle get_stdin()  { return FileHandle { 0, 0 }; }
    constexpr FileHandle get_stdout() { return FileHandle { 0, 1 }; }
    constexpr FileHandle get_stderr() { return FileHandle { 0, 2 }; }
}