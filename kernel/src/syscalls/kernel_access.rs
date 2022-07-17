use crate::memory::allocator::PageAllocator;
use crate::memory::paging::PageFrame;
use crate::spinlock::Lock;

pub struct KernelObjects
{
    allocator: *mut PageAllocator,
    page_frame: *mut PageFrame
}

impl KernelObjects
{
    const fn default() -> KernelObjects
    {
        KernelObjects {
            allocator: 0 as *mut _,
            page_frame: 0 as *mut _
        }
    }

    pub fn set(&mut self, allocator: &mut PageAllocator, frame: &mut PageFrame)
    {
        self.allocator = allocator;
        self.page_frame = frame;
    }

    pub fn allocator(&self) -> &mut PageAllocator
    {
        unsafe { &mut *self.allocator }
    }

    pub fn page_frame(&self) -> &mut PageFrame
    {
        unsafe { &mut *self.page_frame }
    }
}

pub(crate) static KERNEL: Lock<KernelObjects> = Lock::new(KernelObjects::default());