use core::sync::atomic::AtomicBool;
use core::sync::atomic::Ordering;
use core::cell::UnsafeCell;

pub struct Lock<T>
{
    active: AtomicBool,
    data: UnsafeCell<T>
}

impl<T> Lock<T>
{
    pub const fn new(data: T) -> Lock<T>
    {
        Lock
        {
            active: AtomicBool::new(false),
            data: UnsafeCell::new(data)
        }
    }

    pub fn lock(&self) -> &mut T
    {
        /*
            Obviously on a proper multi-core system, the below would be written:
            while self.active.load(Ordering::Acquire) {}

            However as of now only a single core is supported, and so if we find
            that we're locked out it's just going to hang everything, so can be
            treated as an error.
        */
        if self.active.load(Ordering::Acquire) { panic!("spinlock already locked"); }
        self.active.store(true, Ordering::Release);

        // To allow for non-mutable static variables, "pretend" the data's immutable
        unsafe { &mut *self.data.get() }
    }

    pub fn free(&self)
    {
        self.active.store(false, Ordering::Release);
    }
}

unsafe impl<T> Sync for Lock<T> {}