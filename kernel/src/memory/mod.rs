pub mod allocator;
pub mod paging;
mod page_array;

use multiboot2;
use allocator::PageAllocator;
use paging::PageFrame;
use paging::PAGE_SIZE;

extern "C" { pub(crate) static __kernel_end: u32; }

pub fn init(multiboot_info: &multiboot2::BootInformation) -> (PageAllocator, PageFrame)
{
    unsafe
    {
        // Create allocator
        let (allocator,
            memory_end,
            page_frame_address) = PageAllocator::create_root_allocator(&multiboot_info);

        // Create root paging frame
        let mut kernel_frame = PageFrame::create_kernel_frame(page_frame_address);

        // Identity map all pages for the kernel
        let pages = memory_end / PAGE_SIZE;
        for i in 0..pages
        {
            kernel_frame.map_page(i * PAGE_SIZE, i * PAGE_SIZE, false);
        }

        // Enable paging
        kernel_frame.load_to_cpu();
        crate::arch::enable_paging();

        (allocator, kernel_frame)
    }
}

pub fn create_user_page_frame(allocator: &mut PageAllocator) -> PageFrame
{
    let mut frame = paging::PageFrame::create_user_frame(allocator);

    // Map kernel into memory first, so interrupts and the like can run
    unsafe
    {
        let kernel_end = &__kernel_end as *const _ as usize;
        let pages = kernel_end / PAGE_SIZE;

        for i in 0..pages {
            frame.map_page(i * PAGE_SIZE, i * PAGE_SIZE, false);
        }
    }

    frame
}