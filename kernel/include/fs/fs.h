#pragma once
#include "memory/multiboot_info.h"

namespace fs
{
    typedef int FileDescriptor;

    struct FileHandle
    {
        size_t file_system;
        FileDescriptor fd;
    };

    class FileSystem
    {
    public:
        virtual Optional<uint64_t> write(FileDescriptor fd, void* data, uint64_t offset, uint64_t length) = 0;
        virtual Optional<uint64_t> read(FileDescriptor fd, void* data, uint64_t offset, uint64_t length) = 0;
        virtual Optional<uint64_t> get_size(FileDescriptor fd) = 0;
        virtual Optional<FileDescriptor> get_file(const char* path) = 0;
    };

    void init(const memory::MultibootInfo& info);
    Optional<uint64_t> write(FileHandle handle, void* data, uint64_t offset, uint64_t length);
    Optional<uint64_t> read(FileHandle handle, void* data, uint64_t offset, uint64_t length);
    Optional<uint64_t> get_size(FileHandle handle);
    Optional<FileHandle> get_file(const char* path);

    constexpr FileHandle get_stdin()  { return FileHandle { 0, 0 }; }
    constexpr FileHandle get_stdout() { return FileHandle { 0, 1 }; }
    constexpr FileHandle get_stderr() { return FileHandle { 0, 2 }; }
}