mod stack;
mod paging;

use multiboot2;
use multiboot2::{MemoryArea, MemoryAreaType};
use stack::Stack;
use paging::PageFrame;
use crate::arch;

pub const PAGE_SIZE: usize = 4096;
extern "C" { pub static __kernel_end: u32; }

#[derive(Copy, Clone)]
struct FreePage
{
    address: usize
}

pub struct PageAllocator
{
    free_pages: &'static mut Stack<FreePage>,
    page_frame: PageFrame,
    pub free_pages_count: usize
}

impl PageAllocator
{
    pub fn create_root_allocator(multiboot_info: &multiboot2::BootInformation) -> PageAllocator
    {
        // Get memory range from multiboot info
        assert!(multiboot_info.memory_map_tag().is_some());
        let memory_range = get_memory_range(multiboot_info).unwrap_or_else(|| {
            panic!("No valid regions of memory found");
        });

        // Make sure it's page aligned
        assert!(is_page_aligned(memory_range.start_address() as usize));
        assert!(is_page_aligned(memory_range.end_address() as usize));

        // Leave room for page frame
        let page_frame_size = PageFrame::size();

        // Knowing the chosen region starts beneath the kernel, set the start of memory there
        let kernel_end = unsafe { &__kernel_end as *const _ as usize };
        let mem_start = kernel_end + page_frame_size;
        let mem_end = memory_range.end_address() as usize;
        let pages = (mem_end - mem_start) / PAGE_SIZE - 1;

        unsafe
        {
            // 1) Create ourselves
            let free_pages = Stack::<FreePage>::create_at_address(mem_start, FreePage {
                address: mem_start,
            });

            let page_frame = PageFrame::new(kernel_end);

            let mut this = PageAllocator {
                free_pages: &mut *free_pages,
                page_frame,
                free_pages_count: 0
            };

            // 2) Set pages to allocated, starting at page 1 because the act of creating a page frame already freed one
            for page in 1..pages {
                this.free_page(mem_start + page*PAGE_SIZE);
            }

            // 3) Map everything up to the kernel into memory so we don't page fault
            for page in 0..mem_start/PAGE_SIZE {
                this.page_frame.map_page(page * PAGE_SIZE, page * PAGE_SIZE, false);
            }

            // Enable paging
            arch::cpu::load_cr3(kernel_end);
            arch::enable_paging();
            this
        }
    }

    pub fn allocate_page(&mut self, user_page: bool) -> usize
    {
        // Get physical page
        let addr: Option<FreePage> = self.free_pages.pop();
        if addr.is_none() { panic!("no free pages left"); }
        self.free_pages_count -= 1;

        // Map into memory
        let address = addr.unwrap().address;
        unsafe { self.page_frame.map_page(address, address, user_page); }

        address
    }

    pub unsafe fn reserve_page(&mut self, address: usize, user_page: bool)
    {
        assert!(is_page_aligned(address));
        let mut node = Some(self.free_pages as *mut Stack<FreePage>);

        while node.is_some()
        {
            if (*node.unwrap()).data.address == address
            {
                // Set page as used
                (*node.unwrap()).pop();
                self.free_pages_count -= 1;

                // Map into memory
                 self.page_frame.map_page(address, address, user_page);
                return;
            }

            node = (*node.unwrap()).next;
        }

        panic!("could not find reserved page");
    }

    pub unsafe fn free_page(&mut self, address: usize)
    {
        self.free_pages.push(address, FreePage {
            address,
        });
        self.free_pages_count += 1;
    }
}

pub fn is_page_aligned(size: usize) -> bool
{
    size % PAGE_SIZE == 0
}

fn get_memory_range(multiboot_info: &multiboot2::BootInformation) -> Option<&MemoryArea>
{
    // Attempt to find a contiguous area of memory in which to operate...
    let heap_begin = unsafe { &__kernel_end as *const _ as usize };

    let mut valid_areas = multiboot_info.memory_map_tag().unwrap()
        .all_memory_areas()
        .filter(|f| f.typ() == MemoryAreaType::Available)
        .filter(|f| (f.start_address() as usize) < heap_begin)
        .filter(|f| (f.end_address() as usize) > heap_begin);

    valid_areas.nth(0)
}