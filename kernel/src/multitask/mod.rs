pub mod module;
pub mod scheduler;
pub mod task;
mod elf;

pub fn add_task(task: task::Task)
{
    scheduler::SCHEDULER.lock().add_task(task);
    scheduler::SCHEDULER.free();
}