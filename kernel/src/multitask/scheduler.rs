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

pub(crate) struct Scheduler
{
    tasks: [Option<Task>; MAX_TASKS],
    number_of_tasks: usize,
    current_task: usize,
    kernel_switch_performed: bool,
    assembly_info: AssemblyInfo
}

pub(crate) static SCHEDULER: Lock<Scheduler> = Lock::new(Scheduler::default());
const MAX_TASKS: usize = 32;

impl Scheduler
{
    #[no_mangle]
    extern "C" fn on_schedule_interrupt() -> usize
    {
        let this = SCHEDULER.lock();
        assert!(this.number_of_tasks > 0);

        // Choose next task
        let previous_task = this.current_task;
        this.current_task += 1;
        if this.current_task >= this.number_of_tasks {
            this.current_task = 0;
        }

        // End interrupt
        pit::reload();
        pic::end_interrupt(0);

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

