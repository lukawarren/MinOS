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

pub fn init(allocator: allocator::PageAllocator, page_frame: paging::PageFrame)
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

        // k_increase_heap(size_t size)
        2 =>
        {
            let size = registers.ebx as usize;
            k_increase_heap(kernel, task, size)
        },

        // k_decrease_heap(size_t size)
        3 =>
        {
            let size = registers.ebx as usize;
            k_decrease_heap(kernel, task, size)
        },

        4 => // k_exit()
        {
            exited = true;
            0
        }

        _ => panic!("unknown syscall {}", syscall_id)
    };

    SCHEDULER.free();
    KERNEL.free();

    // If task did exist, we need to pick a new one
    if exited {
        scheduler.remove_current_task();
        Scheduler::change_tasks(false);
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

fn k_increase_heap(kernel: &kernel_access::KernelObjects, task: &mut Task, size: usize) -> usize
{
    if size == 0 { return task.heap_start + task.heap_size }

    let initial_heap_end = task.heap_start + task.heap_size;
    let new_heap_end = initial_heap_end + size;
    let pages = (new_heap_end / paging::PAGE_SIZE) - (initial_heap_end / paging::PAGE_SIZE);

    if pages > 0
    {
        let aligned = paging::round_up_to_nearest_page(initial_heap_end);

        for i in 0..pages
        {
            // Map in new pages
            let _ = kernel.allocator().allocate_user_raw_with_address(
                aligned + i * paging::PAGE_SIZE,
                paging::PAGE_SIZE,
                &mut task.page_frame
            );
        }
    }

    // Return start of new memory region allocated
    task.heap_size += size;
    new_heap_end
}

fn k_decrease_heap(kernel: &kernel_access::KernelObjects, task: &mut Task, size: usize) -> usize
{
    if size == 0 { return task.heap_start + task.heap_size }

    assert!(paging::is_page_aligned(task.heap_size));
    assert!(paging::is_page_aligned(task.heap_start));
    assert!(paging::is_page_aligned(size));

    // Unmap from memory
    let new_size = task.heap_size - size;
    kernel.allocator().deallocate_user(task.heap_start + new_size, size, &mut task.page_frame);
    task.heap_size = new_size;

    // Return new end of memory
    return task.heap_start + task.heap_size
}