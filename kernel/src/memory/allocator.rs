#![allow(dead_code)]

use super::stack::Stack;
use super::paging::PageFrame;
use super::paging::PAGE_SIZE;
use super::paging::is_page_aligned;
use crate::memory::paging::round_up_to_nearest_page;
use multiboot2::{BootInformation, MemoryArea, MemoryAreaType};
use crate::multitask::module;
use crate::println;

extern "C" { pub static __kernel_end: u32; }

#[derive(Copy, Clone)]
struct FreePage
{
    physical_address: usize
}

pub struct PageAllocator
{
    free_pages: &'static mut Stack<FreePage>,
    pub free_pages_count: usize,
    pub page_frame_address: usize,
    pub heap_start_address: usize,
    pub heap_end_address: usize
}

impl PageAllocator
{
    pub fn create_root_allocator(multiboot_info: &BootInformation) -> PageAllocator
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
        let mut kernel_end = unsafe { &__kernel_end as *const _ as usize };

        // Anywhere from kernel_end to mem_start is going to be overwritten, so make sure it
        // won't overwrite the multiboot stuff
        if kernel_end < multiboot_info.start_address() &&
            kernel_end + page_frame_size >= multiboot_info.start_address()
        {
            kernel_end = round_up_to_nearest_page(multiboot_info.end_address() + 1);
        }

        else if kernel_end >= multiboot_info.start_address() { panic!(); }

        // Likewise skip over modules - TODO: stop assuming modules won't be high up in memory
        let highest_module_address = module::highest_module_address(multiboot_info);
        if kernel_end < highest_module_address
        {
            kernel_end = round_up_to_nearest_page(highest_module_address + 1);
        }

        // ...and finally start the heap in safety
        let mem_start = kernel_end + page_frame_size;
        let mem_end = memory_range.end_address() as usize;
        let heap_pages = (mem_end - mem_start) / PAGE_SIZE;

        assert_eq!(module::address_lies_within_module(kernel_end, multiboot_info), false);
        assert_eq!(module::address_lies_within_module(mem_start, multiboot_info), false);

        println!("[Memory] Multiboot header lies at {:#x}-{:#x}", multiboot_info.start_address(), multiboot_info.end_address());
        println!("[Memory] Creating root allocator with range {:#x}-{:#x} from {:#x}", mem_start, mem_end, kernel_end);

        unsafe
        {
            // 1) Create structure to keep track of free pages
            let free_pages = &mut *Stack::<FreePage>::create_at_address(mem_start, FreePage {
                physical_address: mem_start,
            });

            // 2) Create ourselves
            let mut this = PageAllocator {
                free_pages,
                free_pages_count: 0,
                page_frame_address: kernel_end,
                heap_start_address: mem_start,
                heap_end_address: mem_end
            };

            // Set all pages following the kernel to free, avoiding page 1 because the act of
            // creating a page frame already freed it... except go in reverse so that we end up
            // allocating pages from the start of memory to the end (because it looks nicer)...
            // except avoid setting pages to free if they overlap with a multiboot module...
            // ...or the multiboot header itself

            for i in 0..heap_pages
            {
                let page = heap_pages - i;
                let physical_address = mem_start + page * PAGE_SIZE;

                if !module::address_lies_within_module(physical_address, multiboot_info) &&
                    !(physical_address >= multiboot_info.start_address() && physical_address <= multiboot_info.end_address())
                {
                    this.free_pages.push(physical_address, FreePage {
                        physical_address,
                    });
                    this.free_pages_count += 1;
                }
            }

            this
        }
    }

    /// Returns virtual address
    pub fn allocate_kernel_page(&mut self) -> usize
    {
        // No need to map into memory afterwards because kernel pages are always mapped
        // anyway (because the linked list of free pages is stored in the pages themselves)
        let addr: Option<FreePage> = self.free_pages.pop();
        if addr.is_none() { panic!("no free pages left"); }
        self.free_pages_count -= 1;
        addr.unwrap().physical_address
    }

    /// Returns virtual address
    pub fn allocate_user_page(&mut self, page_frame: &mut PageFrame) -> usize
    {
        // Allocate as if it was for the kernel, then map into memory manually (see above)
        let address = self.allocate_kernel_page();
        unsafe { page_frame.map_page(address, address, true); }
        address
    }

    /// Returns physical address
    pub fn allocate_user_page_with_address(&mut self, virtual_address: usize, page_frame: &mut PageFrame) -> usize
    {
        let physical_address = self.allocate_kernel_page();
        unsafe { page_frame.map_page(physical_address, virtual_address, true); }
        physical_address
    }

    pub unsafe fn free_kernel_page(&mut self, virtual_address: usize, page_frame: &mut PageFrame)
    {
        assert!(is_page_aligned(virtual_address));

        // Add to list of free pages and be done (no need to touch any page frames - see above)
        let physical_address = page_frame.virtual_address_to_physical(virtual_address);
        self.free_pages.push(physical_address, FreePage {
            physical_address,
        });
        self.free_pages_count += 1;
    }

    pub unsafe fn free_user_page(&mut self, virtual_address: usize, page_frame: &mut PageFrame)
    {
        // Treat as if kernel page, then unmap (see above)
        self.free_kernel_page(virtual_address, page_frame);
        page_frame.unmap_page(virtual_address);
    }

    pub fn allocate_user_page_frame(&mut self) -> PageFrame
    {
        PageFrame::create_user_frame(self)
    }
}

fn get_memory_range(multiboot_info: &BootInformation) -> Option<&MemoryArea>
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
