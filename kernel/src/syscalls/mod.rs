mod kernel_access;

use crate::multitask::scheduler::Scheduler;
use crate::multitask::scheduler::SCHEDULER;
use crate::multitask::task::Task;
use crate::memory::allocator;
use crate::memory::paging;
use kernel_access::KERNEL;
use crate::arch::cpu;
use crate::print;

/// For returning to assembly
#[repr(C, packed)]
struct SyscallReturn {
    return_value: usize,
    task_did_exist: bool
}
extern "C" {
    static mut syscall_return: SyscallReturn;
}

pub fn init(allocator: &mut allocator::PageAllocator, page_frame: &mut paging::PageFrame)
{
    KERNEL.lock().set(allocator, page_frame);
    KERNEL.free();
}

#[no_mangle]
extern "C" fn on_syscall(registers: cpu::Registers)
{
    let syscall_id = registers.eax;
    let kernel = KERNEL.lock();
    let scheduler = SCHEDULER.lock();
    let task = scheduler.get_current_task();

    let mut exited = false;
    let return_value = match syscall_id
    {
        // k_error(char const* message, size_t len)
        0 => {
            print_from_syscall(kernel, task, &registers);
            panic!("see above");
        },

        // print_string(char const* message, size_t len)
        1 => {
            print_from_syscall(kernel, task, &registers);
            0
        },

        2 => // k_exit()
        {
            exited = true;
            0
        },

        // k_allocate_pages(size_t pages)
        3 =>
        {
            let pages = registers.ebx as usize;
            k_allocate_pages(kernel, task, pages)
        },

        // k_free_pages(size_t address, size_t pages)
        4 =>
        {
            let address = registers.ebx as usize;
            let pages = registers.ecx as usize;
            k_free_pages(kernel, task, address, pages);
            0
        }

        _ => panic!("unknown syscall {}", syscall_id)
    };

    SCHEDULER.free();
    KERNEL.free();

    // If task did exist, we need to pick a new one
    if exited {
        scheduler.remove_current_task(kernel.allocator());
        Scheduler::change_tasks(false);
        crate::println!("Free pages is now {}", kernel.allocator().free_pages());
    }

    unsafe
    {
        syscall_return.return_value = return_value;
        syscall_return.task_did_exist = exited;
    }
}

fn print_from_syscall(kernel: &kernel_access::KernelObjects, task: &Task, registers: &cpu::Registers)
{
    let address = registers.ebx as usize;
    let size = registers.ecx as usize;
    let message = kernel.page_frame().read_from_user_memory(address, size, &task.page_frame).unwrap();

    for i in 0..size
    {
        let char = unsafe { *((message + i) as *const u8) };
        print!("{}", char as char);
    }
}

fn k_allocate_pages(kernel: &kernel_access::KernelObjects, task: &mut Task, pages: usize) -> usize
{
    assert!(pages > 0);
    kernel.allocator().allocate_user_raw(paging::PAGE_SIZE * pages, &mut task.page_frame)
}

fn k_free_pages(kernel: &kernel_access::KernelObjects, task: &mut Task, address: usize, pages: usize)
{
    assert!(pages > 0);
    kernel.allocator().deallocate_user(address, paging::PAGE_SIZE * pages, &mut task.page_frame);
}