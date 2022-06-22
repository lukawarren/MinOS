mod kernel_access;

use crate::multitask::scheduler::SCHEDULER;
use crate::interrupts::subscribe_to_irq;
use kernel_access::KERNEL;
use crate::arch::cpu;
use crate::memory;
use crate::print;

pub fn init(allocator: memory::allocator::PageAllocator, page_frame: memory::paging::PageFrame)
{
    KERNEL.lock().set(allocator, page_frame);
    subscribe_to_irq(80, on_syscall);
    KERNEL.free();
}

fn on_syscall(registers: &cpu::Registers)
{
    let syscall_id = registers.eax;
    let kernel = KERNEL.lock();
    let task = SCHEDULER.lock().get_current_task();

    match syscall_id
    {
        0 => // print_line(char* message, size_t len)
        {
            let address = registers.ebx as usize;
            let size = registers.ecx as usize;
            let message = kernel.page_frame().read_from_user_memory(address, size, &task.page_frame).unwrap();

            for i in 0..size
            {
                let char = unsafe { *((message + i) as *const u8) };
                print!("{}", char as char);
            }
            print!("\n");
        }

        _ => panic!("unknown syscall")
    }

    SCHEDULER.free();
    KERNEL.free();
}