#include "fs/device_fs.h"

namespace fs
{
    Optional<FileDescriptor> DeviceFileSystem::install(const DeviceFile& file)
    {
        if (n_files < max_files)
        {
            files[n_files] = file;
            return n_files++;
        }

        println("too many devices - ", n_files);
        assert(false);
        return {};
    }

    Optional<uint64_t> DeviceFileSystem::write(FileDescriptor fd, void* data, uint64_t offset, uint64_t length)
    {
        if (fd >= n_files) return {};
        return files[fd].write(data, offset, length);
    }

    Optional<uint64_t> DeviceFileSystem::read(FileDescriptor fd, void* data, uint64_t offset, uint64_t length)
    {
        if (fd >= n_files) return {};
        return files[fd].read(data, offset, length);
    }

    Optional<uint64_t> DeviceFileSystem::get_size(FileDescriptor fd)
    {
        assert(false);
        return __UINT64_MAX__;
    }

    Optional<FileDescriptor> DeviceFileSystem::get_file(const char* path)
    {
        for (FileDescriptor i = 0; i < n_files; ++i)
            if (strcmp(files[i].name, path) != 0)
                return (FileDescriptor)i;

        return {};
    }
}