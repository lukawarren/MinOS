#pragma once
#include "fs/types.h"

namespace fs
{
    class DeviceFile
    {
    public:
        DeviceFile(
            Optional<uint64_t> (*on_read)(void*, uint64_t, uint64_t),
            Optional<uint64_t> (*on_write)(void*, uint64_t, uint64_t),
            const char* _name
        )
        {
            read = on_read;
            write = on_write;

            const auto max_length = sizeof(this->name)/sizeof(this->name[0]);
            assert(strlen(_name) <= max_length);
            strncpy(this->name, _name, max_length);
        }

        DeviceFile() {}
        ~DeviceFile() {}

        Optional<uint64_t> (*read)(void*, uint64_t, uint64_t);
        Optional<uint64_t> (*write)(void*, uint64_t, uint64_t);
        char name[32];
    };

    class DeviceFileSystem : public FileSystem
    {
    public:
        Optional<FileDescriptor> install(const DeviceFile& file);
        Optional<uint64_t> write(FileDescriptor fd, void* data, uint64_t offset, uint64_t length) override;
        Optional<uint64_t> read(FileDescriptor fd, void* data, uint64_t offset, uint64_t length) override;
        Optional<uint64_t> get_size(FileDescriptor fd) override;
        Optional<FileDescriptor> get_file(const char* path) override;

    private:
        constexpr static FileDescriptor max_files = 32;
        DeviceFile files[max_files];
        FileDescriptor n_files = 0;
    };
}