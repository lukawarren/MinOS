#pragma once
#include "klib.h"

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
}