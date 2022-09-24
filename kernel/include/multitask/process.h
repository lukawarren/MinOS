#pragma once
#include "klib.h"
#include "minlib.h"
#include "memory/page_frame.h"
#include "fs/fs.h"
#include <unistd.h>

namespace multitask
{
    using fd = fs::FileDescriptor;

    class Process
    {
    public:
        Process(memory::PageFrame page_frame, const size_t entrypoint);
        Process() {}

        enum SeekMode { SET = 0, CURRENT = 1, END = 2 };

        // Files
        Optional<fd> open_file(const fs::FileHandle handle);
        Optional<fd> open_file(const char* path);
        Optional<uint64_t> seek_file(const fd fd, const uint64_t offset, const SeekMode mode);
        bool close_file(const fd fd);
        bool is_fd_valid(const fd fd) const;

        // Kernel + syscalls
        memory::PageFrame frame;
        pid_t thread_id;

        // Files
        static constexpr fd max_files = 16;
        struct OpenFile
        {
            fs::FileHandle handle;
            uint64_t offset;
            uint64_t size() const {
                return fs::get_size(handle).value_or(0);
            }
        };
        OpenFile open_files[max_files];
        fd n_files = 0;

        // IPC message system
        const static size_t max_messages = 32;
        Message messages[max_messages];
        unsigned int n_messages = 0;

        static_assert(sizeof(Message) == 128);
        static_assert(sizeof(Message) * max_messages == PAGE_SIZE);
        
        bool add_message(const Message& message);
        Optional<Message> get_message();

        // Scheduler variables
        size_t esp;
        char* fxsave_storage; // for fxsave and fxrstor
    };
}