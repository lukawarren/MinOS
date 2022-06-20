use bitflags::bitflags;
use core::mem::size_of;
use crate::arch;
use super::allocator::PageAllocator;

pub const PAGE_SIZE: usize = 4096;
const PAGE_TABLES: usize = 1024;
const PAGE_DIRECTORIES: usize = 1024;
const DIRECTORY_SIZE: usize = PAGE_SIZE * PAGE_TABLES;

bitflags!
{
    struct PageFlags : usize
    {
        const PRESENT = 1;
        const READ_WRITE = 1 << 1;
        const GLOBAL_ACCESS = 1 << 2;

        const KERNEL_PAGE_DISABLED = 0;
        const KERNEL_PAGE_READ_WRITE = Self::PRESENT.bits() | Self::READ_WRITE.bits();
        const USER_PAGE_READ_WRITE = Self::PRESENT.bits() | Self::READ_WRITE.bits() | Self::GLOBAL_ACCESS.bits();
    }
}

#[repr(packed)]
#[derive(Default)]
struct PageTable
{
    physical_address_with_flags: usize
}

impl PageTable
{
    fn new() -> PageTable
    {
        // Set page to be disabled and owned by the kernel (by default)
        PageTable {
            physical_address_with_flags: PageFlags::KERNEL_PAGE_DISABLED.bits()
        }
    }

    fn set(&mut self, physical_address: usize, flags: PageFlags)
    {
        assert!(is_page_aligned(physical_address));
        self.physical_address_with_flags = physical_address | flags.bits();
    }

    fn is_set(&self) -> bool
    {
        self.physical_address_with_flags != PageFlags::KERNEL_PAGE_DISABLED.bits()
    }

    fn physical_address(&self) -> usize
    {
        self.physical_address_with_flags & 0xfffff000
    }
}

#[repr(packed)]
#[derive(Default)]
#[allow(dead_code)]
struct PageDirectory
{
    physical_address_with_flags: usize
}

impl PageDirectory
{
    fn new(first_page_table: &PageTable) -> PageDirectory
    {
        let physical_table_address = (first_page_table as *const _) as usize;

        // Set the page directory to be user-space-readable and
        // present, knowing that the page tables themselves are
        // non-present and restricted to the kernel (by default).
        PageDirectory {
            physical_address_with_flags: physical_table_address |
                PageFlags::USER_PAGE_READ_WRITE.bits()
        }
    }

    fn physical_address(&self) -> usize
    {
        self.physical_address_with_flags & 0xfffff000
    }
}

#[allow(dead_code)]
#[repr(packed)]
pub struct PageFrame
{
    directories: &'static mut[PageDirectory; PAGE_DIRECTORIES]
}

impl PageFrame
{
    pub unsafe fn create_kernel_frame(physical_start_address: usize) -> PageFrame
    {
        // Use contiguous directories and tables
        let directories = &mut *(physical_start_address as *mut [PageDirectory; PAGE_DIRECTORIES]);
        let tables = &mut *((physical_start_address + size_of::<PageDirectory>() * PAGE_DIRECTORIES) as *mut [PageTable; PAGE_TABLES * PAGE_DIRECTORIES]);

        // Initialise page directories
        for i in 0..directories.len() {
            directories[i] = PageDirectory::new(&tables[i * PAGE_TABLES]);
        }

        // Initialise page tables
        for i in 0..tables.len() {
            tables[i] = PageTable::new();
        }

        PageFrame {
            directories
        }
    }

    pub fn create_user_frame(allocator: &mut PageAllocator) -> PageFrame
    {
        debug_assert!(size_of::<PageDirectory>() * PAGE_DIRECTORIES <= PAGE_SIZE);
        debug_assert!(size_of::<PageTable>() * PAGE_TABLES <= PAGE_SIZE);

        let directories_address = allocator.allocate_kernel_page();
        let directories = unsafe { &mut *(directories_address as *mut [PageDirectory; PAGE_DIRECTORIES]) };

        // Initialise directories...
        for i in 0..directories.len()
        {
            let tables_address = allocator.allocate_kernel_page();
            let tables = unsafe { &mut *(tables_address as *mut [PageTable; PAGE_TABLES]) };

            // ...and tables
            for j in 0..tables.len() {
                tables[j] = PageTable::new();
            }

            directories[i] = PageDirectory::new(&tables[0]);
        }

        PageFrame {
            directories
        }
    }

    pub unsafe fn map_page(&mut self, physical_address: usize, virtual_address: usize, user_page: bool)
    {
        // The physical location of the page table / directory represents the virtual address mapped,
        // and the physical address inside a page table itself represents the physical address mapped

        assert!(is_page_aligned(physical_address));
        assert!(is_page_aligned(virtual_address));

        let flags = if user_page { PageFlags::USER_PAGE_READ_WRITE }
                    else { PageFlags::KERNEL_PAGE_READ_WRITE };

        let table = self.get_table_mut(virtual_address);
        assert_eq!(table.is_set(), false);
        table.set(physical_address, flags);

        arch::flush_tlb();
    }

    pub unsafe fn unmap_page(&mut self, virtual_address: usize)
    {
        assert!(is_page_aligned(virtual_address));

        // Revert back to default mapping, disabling the page
        *self.get_table_mut(virtual_address) = PageTable::new();
        arch::flush_tlb();
    }

    pub unsafe fn load_to_cpu(&self)
    {
        let physical_start_address = (&self.directories[0] as *const _) as usize;
        arch::cpu::load_cr3(physical_start_address);
    }

    pub fn virtual_address_to_physical(&self, address: usize) -> usize
    {
        let offset = address % PAGE_SIZE;
        unsafe { self.get_table(address).physical_address() + offset }
    }

    pub fn size() -> usize
    {
        size_of::<PageDirectory>() * PAGE_DIRECTORIES +
        size_of::<PageTable>() * PAGE_TABLES * PAGE_DIRECTORIES
    }

    unsafe fn get_table_mut(&mut self, virtual_address: usize) -> &mut PageTable
    {
        let page_directory = virtual_address / DIRECTORY_SIZE;
        let page_table = (virtual_address / PAGE_SIZE) % PAGE_TABLES;

        let tables_address = self.directories[page_directory].physical_address(); // Assumes physical address to tables is also the virtual,
                                                                                  // because for now the kernel is identity mapped

        let tables = &mut *(tables_address as *mut [PageTable; PAGE_TABLES]);

        &mut tables[page_table]
    }

    unsafe fn get_table(&self, virtual_address: usize) -> &PageTable
    {
        let page_directory = virtual_address / DIRECTORY_SIZE;
        let page_table = (virtual_address / PAGE_SIZE) % PAGE_TABLES;
        let tables_address = self.directories[page_directory].physical_address(); // See above
        let tables = &*(tables_address as *const [PageTable; PAGE_TABLES]);
        &tables[page_table]
    }
}

pub fn is_page_aligned(size: usize) -> bool
{
    size % PAGE_SIZE == 0
}

pub fn round_up_to_nearest_page(address: usize) -> usize
{
    let remainder = address % PAGE_SIZE;
    if remainder == 0 { address } else { address + PAGE_SIZE - remainder }
}