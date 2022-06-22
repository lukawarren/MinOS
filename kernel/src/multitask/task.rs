use crate::memory::paging;
use crate::memory::allocator;
use crate::arch::cpu::USER_DATA_SEGMENT;
use crate::arch::cpu::USER_CODE_SEGMENT;
use core::mem;

const TASK_STACK_SIZE: usize = 8192;
const TASK_STACK_ENTRIES: usize = TASK_STACK_SIZE / core::mem::size_of::<usize>();

#[derive(Clone, Copy)]
pub struct Task
{
    pub stack_address: usize,
    pub page_frame: paging::PageFrame
}

struct Stack
{
    bytes: [usize; TASK_STACK_ENTRIES]
}

impl Default for Stack {
    fn default() -> Self {
        Stack {
            bytes: [0; TASK_STACK_ENTRIES]
        }
    }
}

impl Task
{
    pub fn create_task(allocator: &mut allocator::PageAllocator, mut frame: paging::PageFrame, entrypoint: usize) -> Task
    {
        // Create 16-byte aligned stack, growing downwards
        let stack = allocator.allocate_user::<Stack>(&mut frame);
        let mut stack_pointer = TASK_STACK_ENTRIES - 16; // In usize's

        // Check we'll be 16-byte aligned
        let stack_address = (stack as *const _) as usize + stack_pointer * mem::size_of::<usize>();
        assert_eq!(stack_address % 16, 0);

        // Setup stack
        let mut push = |data| {
            stack.bytes[stack_pointer] = data;
            stack_pointer -= 1;
        };

        // Frame for iret
        push(0x0);                                  // stack alignment (if any)
        push(USER_DATA_SEGMENT as usize);           // stack segment (ss)
        push(stack_address);                        // esp
        push(0x202);                                // eflags - default value with interrupts enabled
        push(USER_CODE_SEGMENT as usize);           // cs
        push(entrypoint);                           // eip

        // Registers restored in context switch
        push(0);                                    // eax
        push(0);                                    // ecx
        push(0);                                    // edx
        push(0);                                    // ebx
        push(0);                                    // esp
        push(0);                                    // ebp
        push(0);                                    // esi
        push(0);                                    // edi

        // Other segment registers
        push(USER_DATA_SEGMENT as usize);           // ds
        push(USER_DATA_SEGMENT as usize);           // fs
        push(USER_DATA_SEGMENT as usize);           // es
        push(USER_DATA_SEGMENT as usize);           // gs

        // CR3
        push(frame.cr3());

        // Final push, so "un-push" in preparation
        stack_pointer += 1;

        Task {
            page_frame: frame,
            stack_address: (stack as *const _) as usize + stack_pointer * mem::size_of::<usize>()
        }
    }
}