pub mod allocator;
pub mod paging;
mod page_array;

use multiboot2;

pub fn init(multiboot_info: &multiboot2::BootInformation) -> (allocator::PageAllocator, paging::PageFrame)
{
    unsafe
    {
        // Create allocator and frame
        let (allocator, memory_end, page_frame_address) = allocator::PageAllocator::create_root_allocator(&multiboot_info);
        let mut kernel_frame = paging::PageFrame::create_kernel_frame(page_frame_address);

        // Identity map all pages for the kernel
        let pages = memory_end / paging::PAGE_SIZE;
        for i in 0..pages {
            kernel_frame.map_page(i * paging::PAGE_SIZE, i * paging::PAGE_SIZE, false);
        }

        // Enable paging
        kernel_frame.load_to_cpu();
        crate::arch::enable_paging();

        (allocator, kernel_frame)
    }
}