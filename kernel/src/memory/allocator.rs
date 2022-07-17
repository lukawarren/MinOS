#![allow(dead_code)]

use super::paging::{ PageFrame, PAGE_SIZE, MAX_PAGES, is_page_aligned, round_up_to_nearest_page };
use super::page_array::PageArray;
use multiboot2::{BootInformation, MemoryArea, MemoryAreaType};
use crate::multitask::module;
use crate::println;
use core::ptr;
use core::mem;

pub struct PageAllocator
{
    page_array: PageArray
}

impl PageAllocator
{
    /// Returns allocator followed by the address left for the kernel page frame to go into
    pub fn create_root_allocator(multiboot_info: &BootInformation) -> (PageAllocator, usize)
    {
        // Get memory range from multiboot info
        assert!(multiboot_info.memory_map_tag().is_some());
        let memory_range = get_memory_range(multiboot_info).unwrap_or_else(|| {
            panic!("no valid regions of memory found");
        });

        // Make sure it's page aligned
        assert!(is_page_aligned(memory_range.start_address() as usize));
        assert!(is_page_aligned(memory_range.end_address() as usize));

        // Leave room for page frame and page array
        let kernel_end = unsafe { &super::__kernel_end as *const _ as usize };
        let structures_size = PageFrame::size() + PageArray::size();
        let mut mem_start = kernel_end;

        // Anywhere after mem_start is going to be overwritten,
        // so make sure it won't overwrite the multiboot stuff
        if mem_start + structures_size >= multiboot_info.start_address() {
            mem_start = round_up_to_nearest_page(multiboot_info.end_address() + 1);
        }

        // Likewise skip over modules - TODO: stop assuming modules won't be high up in memory
        let highest_module_address = module::highest_module_address(multiboot_info);
        if mem_start < highest_module_address {
            mem_start = round_up_to_nearest_page(highest_module_address + 1);
        }

        // Finally place things into memory
        let page_array_address = mem_start;
        let page_frame_address = mem_start + PageArray::size();
        let heap_start = page_frame_address + PageFrame::size();

        // Sanity checks
        assert_eq!(module::address_lies_within_module(kernel_end, multiboot_info), false);
        assert_eq!(module::address_lies_within_module(mem_start, multiboot_info), false);
        assert!(is_page_aligned(page_array_address));
        assert!(is_page_aligned(page_frame_address));
        assert!(is_page_aligned(heap_start));
        assert!(is_page_aligned(kernel_end));

        println!("[Memory] Multiboot header lies at {:#x}-{:#x}", multiboot_info.start_address(), multiboot_info.end_address());
        println!("[Memory] Creating structures with range {:#x}-{:#x}", mem_start, heap_start-1);

        unsafe
        {
            // 1) Create structure to keep track of free pages
            let page_array = PageArray::create_at_address(page_array_address);

            // 2) Create ourselves
            let mut this = PageAllocator {
                page_array
            };

            // 3) Set all pages following the kernel to free, except avoid setting pages to free
            //    if they overlap with a multiboot module... or the multiboot header itself!

            let mem_end = memory_range.end_address() as usize;
            let free_pages = (mem_end - heap_start) / PAGE_SIZE;

            for i in 0..free_pages
            {
                let physical_address = heap_start + i * PAGE_SIZE;

                if !module::address_lies_within_module(physical_address, multiboot_info) &&
                    !(physical_address >= multiboot_info.start_address() && physical_address <= multiboot_info.end_address())
                {
                    this.page_array.free_pages(physical_address, 1);
                }
            }

            (this, page_frame_address)
        }
    }

    /// Returns physical address - NOTE: may break if the kernel is not identity mapped
    pub fn allocate_kernel_raw(&mut self, size: usize) -> usize
    {
        let size_aligned = round_up_to_nearest_page(size);
        let address = self.page_array.allocate_pages(size_aligned / PAGE_SIZE);
        address
    }

    /// Returns virtual address
    pub fn allocate_user_raw(&mut self, size: usize, page_frame: &mut PageFrame) -> usize
    {
        // Allocate, then map into memory
        let address = self.allocate_kernel_raw(size);

        unsafe
        {
            let pages = round_up_to_nearest_page(size) / PAGE_SIZE;

            for i in 0..pages {
                page_frame.map_page(address + PAGE_SIZE * i, address + PAGE_SIZE * i, true);
            }
        }

        address
    }

    /// Returns physical address
    pub fn allocate_user_raw_with_address(&mut self, virtual_address: usize, size: usize, page_frame: &mut PageFrame) -> usize
    {
        let physical_address = self.allocate_kernel_raw(size);

        unsafe
        {
            let pages = round_up_to_nearest_page(size) / PAGE_SIZE;

            for i in 0..pages {
                page_frame.map_page(physical_address + PAGE_SIZE * i, virtual_address + PAGE_SIZE * i, true);
            }
        }

        physical_address
    }

    /// Similar to allocate_kernel_raw, though automatically casts to desired type.
    /// Does not manually zero-out memory, instead electing to call T::default()
    pub fn allocate_kernel<T: Default>(&mut self) -> &mut T
    {
        self.create_object_at_address::<T>().0
    }

    /// As with allocate_kernel, but maps into user memory too
    pub fn allocate_user<T: Default>(&mut self, page_frame: &mut PageFrame) -> &mut T
    {
        let (object, address) = self.create_object_at_address::<T>();

        // Map into memory
        unsafe
        {
            let pages = round_up_to_nearest_page(mem::size_of::<T>()) / PAGE_SIZE;
            for i in 0..pages {
                page_frame.map_page(address + PAGE_SIZE * i, address + PAGE_SIZE * i, true);
            }
        }

        object
    }

    pub fn deallocate_user(&mut self, virtual_address: usize, size: usize, page_frame: &mut PageFrame)
    {
        // Make sure it's not kernel memory
        assert!(page_frame.is_user_page(virtual_address));
        let size_aligned = round_up_to_nearest_page(size);
        let pages = size_aligned / PAGE_SIZE;

        // Free from allocator
        let physical_address = page_frame.virtual_address_to_physical(virtual_address);
        self.page_array.free_pages(physical_address, pages);

        // Unmap from page frame
        for i in 0..pages {
            unsafe { page_frame.unmap_page(virtual_address + i * PAGE_SIZE); }
        }

        // Zero out memory for security
        unsafe {
            ptr::write_bytes(physical_address as *mut u8, 0, size_aligned);
        }
    }

    /// Returns the number of free pages (i.e. free the adjective, not the verb)
    pub fn free_pages(&self) -> usize
    {
        self.page_array.number_of_free_pages()
    }

    /// E.g. used when a user task finished and its memory is freed
    pub fn free_user_pages_from_frame(&mut self, page_frame: &mut PageFrame)
    {
        for i in 0..MAX_PAGES
        {
            if page_frame.is_user_page(i * PAGE_SIZE)
            {
                // TODO: write code directly, as technically no need to unmap pages first
                self.deallocate_user(i * PAGE_SIZE, PAGE_SIZE, page_frame);
            }
        }
    }

    /// Returns physical address
    fn create_object_at_address<T: Default>(&mut self) -> (&mut T, usize)
    {
        let size_aligned = round_up_to_nearest_page(mem::size_of::<T>());
        let address = self.page_array.allocate_pages(size_aligned / PAGE_SIZE);

        let object = unsafe { &mut *(address as *mut T) };
        *object = Default::default();

        (object, address)
    }
}

fn get_memory_range(multiboot_info: &BootInformation) -> Option<&MemoryArea>
{
    // Attempt to find a contiguous area of memory in which to operate...
    let heap_begin = unsafe { &super::__kernel_end as *const _ as usize };

    let mut valid_areas = multiboot_info.memory_map_tag().unwrap()
        .all_memory_areas()
        .filter(|f| f.typ() == MemoryAreaType::Available)
        .filter(|f| (f.start_address() as usize) < heap_begin)
        .filter(|f| (f.end_address() as usize) > heap_begin);

    valid_areas.nth(0)
}
