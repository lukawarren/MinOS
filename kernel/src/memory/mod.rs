mod stack;

use multiboot2;
use multiboot2::{MemoryArea, MemoryAreaType};
use stack::Stack;

pub const PAGE_SIZE: usize = 4096;
extern "C" { pub static __kernel_end: u32; }

#[derive(Copy, Clone)]
struct FreePage
{
    address: usize
}

pub struct PageFrame
{
    free_pages: &'static mut Stack<FreePage>,
    pub free_pages_count: usize
}

impl PageFrame
{
    pub fn create_root_frame(multiboot_info: &multiboot2::BootInformation) -> PageFrame
    {
        // Get memory range from multiboot info
        assert!(multiboot_info.memory_map_tag().is_some());
        let memory_range = get_memory_range(multiboot_info).unwrap_or_else(|| {
            panic!("No valid regions of memory found");
        });

        // Make sure it's page aligned
        assert!(is_page_aligned(memory_range.start_address() as usize));
        assert!(is_page_aligned(memory_range.end_address() as usize));

        // Knowing the chosen region starts beneath the kernel, set the start of memory there
        let mem_start = unsafe { &__kernel_end as *const _ as usize };
        let mem_end = memory_range.end_address() as usize;
        let pages = (mem_end - mem_start) / PAGE_SIZE - 1;

        unsafe
        {
            let frame = Stack::<FreePage>::create_at_address(mem_start, FreePage {
                address: mem_start,
            });

            let mut this = PageFrame {
                free_pages: &mut *frame,
                free_pages_count: 0
            };

            // Start at page 1 because the act of creating a page frame already freed one
            for page in 1..pages {
                this.free_page(mem_start + page*PAGE_SIZE);
            }

            this
        }
    }

    pub fn allocate_page(&mut self) -> usize
    {
        let addr: Option<FreePage> = self.free_pages.pop();
        if addr.is_none() { panic!("no free pages left"); }

        self.free_pages_count -= 1;
        addr.unwrap().address
    }

    pub unsafe fn reserve_page(&mut self, address: usize)
    {
        assert!(is_page_aligned(address));
        let mut node = Some(self.free_pages as *mut Stack<FreePage>);

        while node.is_some()
        {
            if (*node.unwrap()).data.address == address
            {
                (*node.unwrap()).pop();
                self.free_pages_count -= 1;
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

pub fn round_size_to_nearest_page(size: usize) -> usize
{
    let remainder = size % PAGE_SIZE;
    if remainder == 0 { size } else { size + PAGE_SIZE - remainder }
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

fn is_page_aligned(address: usize) -> bool
{
    address % PAGE_SIZE == 0
}