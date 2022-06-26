use super::task::Task;
use crate::interrupts::pic;
use crate::interrupts::pit;
use crate::spinlock::Lock;

/// For communicating with the assembly code
#[repr(C, packed)]
struct AssemblyInfo
{
    new_stack_address: usize,
    old_stack_address: usize,
    came_from_kernel: bool
}

pub struct Scheduler
{
    tasks: [Option<Task>; MAX_TASKS],
    number_of_tasks: usize,
    current_task: usize,
    kernel_switch_performed: bool,
    assembly_info: AssemblyInfo
}

pub static SCHEDULER: Lock<Scheduler> = Lock::new(Scheduler::default());
const MAX_TASKS: usize = 32;

impl Scheduler
{
    #[no_mangle]
    pub extern "C" fn change_tasks(came_from_interrupt: bool) -> usize
    {
        let this = SCHEDULER.lock();
        assert!(this.number_of_tasks > 0);

        // Choose next task
        let previous_task = this.current_task;
        this.current_task += 1;
        if this.current_task >= this.number_of_tasks {
            this.current_task = 0;
        }

        // End interrupt if need be (may have been called from a syscall)
        if came_from_interrupt {
            pit::reload();
            pic::end_interrupt(0);
        }

        // Return info for assembly - if this is the first time calling the PIT interrupt, we're
        // coming from the kernel, so we don't have an old task
        this.assembly_info.new_stack_address = &this.tasks[this.current_task].as_ref().unwrap().stack_address as *const _ as usize;
        this.assembly_info.old_stack_address = &this.tasks[previous_task].as_ref().unwrap().stack_address as *const _ as usize;
        this.assembly_info.came_from_kernel = !this.kernel_switch_performed;

        if !this.kernel_switch_performed {
            this.kernel_switch_performed = !this.kernel_switch_performed;
        }

        let addr = &mut this.assembly_info as *mut _ as usize;
        SCHEDULER.free();
        addr
    }

    pub fn add_task(&mut self, task: Task)
    {
        self.tasks[self.number_of_tasks] = Some(task);
        self.number_of_tasks += 1;
    }

    pub fn remove_current_task(&mut self)
    {
        // Move all elements above in the array down by one
        for i in self.current_task..self.number_of_tasks-1 {
            self.tasks[i] = self.tasks[i+1];
        }

        self.number_of_tasks -= 1;
    }

    pub fn get_current_task(&mut self) -> &mut Task
    {
        self.tasks[self.current_task].as_mut().unwrap()
    }

    const fn default() -> Self {
        Scheduler {
            tasks: [None; MAX_TASKS],
            number_of_tasks: 0,
            current_task: 0,
            kernel_switch_performed: false,
            assembly_info: AssemblyInfo {
                new_stack_address: 0,
                old_stack_address: 0,
                came_from_kernel: false
            }
        }
    }
}

