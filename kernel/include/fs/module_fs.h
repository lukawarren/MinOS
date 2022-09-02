#pragma once
#include "fs/fs.h"
#include "memory/multiboot_info.h"

namespace fs
{
    class ModuleFileSystem : public FileSystem
    {
    public:
        ModuleFileSystem() {}
        ModuleFileSystem(const memory::MultibootInfo& info);
        Optional<uint64_t> write(FileDescriptor fd, void* data, uint64_t offset, uint64_t length) override;
        Optional<uint64_t> read(FileDescriptor fd, void* data, uint64_t offset, uint64_t length) override;
        Optional<uint64_t> get_size(FileDescriptor fd) override;
        Optional<FileDescriptor> get_file(const char* path) override;

    private:
        memory::MultibootInfo info;
    };
}