mod stack;
mod paging;

use multiboot2;
use multiboot2::{MemoryArea, MemoryAreaType};
use stack::Stack;
use paging::PageFrame;
use paging::is_page_aligned;
use crate::println;

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
            panic!("no valid regions of memory found");
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
        let heap_pages = (mem_end - mem_start) / paging::PAGE_SIZE;

        println!("[Memory] Initialising with range {:#x}-{:#x}", mem_start, mem_end);

        unsafe
        {
            // 1) Create structure to keep track of free pages
            let free_pages = &mut *Stack::<FreePage>::create_at_address(mem_start, FreePage {
                address: mem_start,
            });

            // 2) Create ourselves, including the page frame
            let mut this = PageAllocator {
                free_pages,
                page_frame: PageFrame::new(kernel_end),
                free_pages_count: 0
            };

            // 3) Set all pages following the kernel to free, avoiding page 1 because the act of
            //    creating a page frame already freed it... except go in reverse so that we
            //    allocate from the start of memory to the end (because why not)
            for i in 0..heap_pages {
                let page = heap_pages - i;
                this.free_page(mem_start + page * paging::PAGE_SIZE);
            }

            // 4) Enable paging
            this.page_frame.load_to_cpu(kernel_end);
            this
        }
    }

    pub fn allocate_page(&mut self, user_page: bool) -> usize
    {
        // Get physical page
        let addr: Option<FreePage> = self.free_pages.pop();
        if addr.is_none() { panic!("no free pages left"); }
        self.free_pages_count -= 1;

        // Identity map into memory
        let address = addr.unwrap().address;
        unsafe { self.page_frame.map_page(address, address, user_page); }

        address
    }

    pub unsafe fn reserve_page(&mut self, physical_address: usize, user_page: bool)
    {
        assert!(is_page_aligned(physical_address));
        let mut node = Some(self.free_pages as *mut Stack<FreePage>);

        while node.is_some()
        {
            if (*node.unwrap()).data.address == physical_address
            {
                // Set page as used
                (*node.unwrap()).pop();
                self.free_pages_count -= 1;

                // Identity map into memory
                 self.page_frame.map_page(physical_address, physical_address, user_page);
                return;
            }

            node = (*node.unwrap()).next;
        }

        panic!("could not find reserved page with address {}", physical_address);
    }

    pub unsafe fn free_page(&mut self, virtual_address: usize)
    {
        // Add to list of free pages
        let physical_address = self.page_frame.virtual_address_to_physical(virtual_address);
        self.free_pages.push(physical_address, FreePage {
            address: physical_address,
        });
        self.free_pages_count += 1;

        // Unmap from memory
        self.page_frame.unmap_page(virtual_address);
    }
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
