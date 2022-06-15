use core::sync::atomic::AtomicBool;
use core::sync::atomic::Ordering;
use core::cell::UnsafeCell;
use crate::println;

pub struct Lock<T>
{
    active: AtomicBool,
    data: UnsafeCell<T>
}

impl<T> Lock<T>
{
    pub fn new(data: T) -> Lock<T>
    {
        Lock
        {
            active: AtomicBool::new(false),
            data: UnsafeCell::new(data)
        }
    }

    pub fn lock(&self) -> &mut T
    {
        // Wait for data to be freed
        while self.active.load(Ordering::Acquire) {}
        self.active.store(true, Ordering::Release);
        println!("TODO: disable interrupts and then re-enable when freeing, and also disable interrupts when handling them"); // TODO

        // To allow for non-mutable static variables, "pretend" the data's immutable
        unsafe { &mut *self.data.get() }
    }

    pub fn free(&self)
    {
        self.active.store(false, Ordering::Release);
    }
}