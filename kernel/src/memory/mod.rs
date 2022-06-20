pub mod allocator;
pub mod paging;
mod stack;

use multiboot2;

pub fn init(multiboot_info: &multiboot2::BootInformation) -> (allocator::PageAllocator, paging::PageFrame)
{
    unsafe
    {
        let allocator = allocator::PageAllocator::create_root_allocator(&multiboot_info);
        let mut kernel_frame = paging::PageFrame::create_kernel_frame(allocator.page_frame_address);

        // Identify map whole memory range, because free pages are stored in linked lists within the
        // pages themselves, so the kernel needs a view of them, even if they're unallocated :-)
        let pages = allocator.heap_end_address / paging::PAGE_SIZE;
        for page in 0..pages
        {
            let addr = page * paging::PAGE_SIZE;
            kernel_frame.map_page(addr, addr, false);
        }

        kernel_frame.load_to_cpu();
        crate::arch::enable_paging();

        (allocator, kernel_frame)
    }
}