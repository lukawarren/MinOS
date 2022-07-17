#![allow(dead_code)]

use bitflags::bitflags;
use core::mem::size_of;
use super::allocator::PageAllocator;
use crate::arch;

pub const PAGE_SIZE: usize = 4096;
pub const MAX_PAGES: usize = 0x100000;

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
        assert!(!self.is_set());

        self.physical_address_with_flags = physical_address | flags.bits();
    }

    fn is_set(&self) -> bool
    {
        self.physical_address_with_flags != PageFlags::KERNEL_PAGE_DISABLED.bits()
    }

    fn is_user_readable(&self) -> bool
    {
        (self.physical_address_with_flags & PageFlags::USER_PAGE_READ_WRITE.bits()) == PageFlags::USER_PAGE_READ_WRITE.bits()
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
#[derive(Clone, Copy)]
#[repr(packed)]
pub struct PageFrame
{
    directories: *mut [PageDirectory; PAGE_DIRECTORIES]
}

impl PageFrame
{
    pub unsafe fn create_kernel_frame(physical_start_address: usize) -> PageFrame
    {
        // Use contiguous directories and tables
        let directories = &mut *(physical_start_address as *mut [PageDirectory; PAGE_DIRECTORIES]);
        let tables = &mut *((physical_start_address + size_of::<PageDirectory>() * PAGE_DIRECTORIES) as *mut [PageTable; PAGE_TABLES * PAGE_DIRECTORIES]);

        // Initialise page directories (identity mapped)
        for i in 0..directories.len() {
            directories[i] = PageDirectory::new(&tables[i * PAGE_TABLES]);
        }

        // Initialise page tables (identity mapped)
        for i in 0..tables.len()
        {
            tables[i] = PageTable::new();
            tables[i].set(i * PAGE_SIZE, PageFlags::KERNEL_PAGE_READ_WRITE);
        }

        // Didn't call normal mapping function so have to flush TLB manually
        arch::flush_tlb();

        // Set cr3 globally for benefit of assembly
        let this = PageFrame { directories };
        super::KERNEL_CR3 = this.cr3();
        this
    }

    pub fn create_user_frame(allocator: &mut PageAllocator) -> PageFrame
    {
        let directories_address = allocator.allocate_kernel_raw(PAGE_SIZE);
        let directories = unsafe { &mut *(directories_address as *mut [PageDirectory; PAGE_DIRECTORIES]) };

        // Initialise directories...
        for i in 0..directories.len()
        {
            let tables_address = allocator.allocate_kernel_raw(PAGE_SIZE);
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

        assert!(is_page_aligned(physical_address), "{:#x} is not aligned", virtual_address);
        assert!(is_page_aligned(virtual_address), "{:#x} is not aligned", virtual_address);

        let flags = if user_page { PageFlags::USER_PAGE_READ_WRITE }
                    else { PageFlags::KERNEL_PAGE_READ_WRITE };


        let table = self.get_table_mut(virtual_address);
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

    /// Attempts to translate a user virtual address into a kernel virtual address (owned by this object),
    /// but only if the referenced pages are user pages that are already mapped in
    pub fn read_from_user_memory(&self, virtual_user_address: usize, size: usize, user_frame: &PageFrame) -> Option<usize>
    {
        // Check pages are valid, mapped user pages
        let beginning = (virtual_user_address/ PAGE_SIZE) * PAGE_SIZE;
        let end = round_up_to_nearest_page(virtual_user_address + size);
        let pages = (end - beginning) / PAGE_SIZE;

        for i in 0..pages
        {
            let page_table = unsafe { user_frame.get_table(virtual_user_address + i * PAGE_SIZE) };
            if page_table.is_user_readable() == false { return None }
        }

        // Assume identity mapping, as does befit the kernel at this point in time
        let physical_address = user_frame.virtual_address_to_physical(virtual_user_address);
        Some(physical_address)
    }

    pub unsafe fn load_to_cpu(&self)
    {
        arch::cpu::load_cr3(self.cr3());
    }

    pub fn cr3(&self) -> usize
    {
        self.directories as usize
    }

    pub fn virtual_address_to_physical(&self, address: usize) -> usize
    {
        let offset = address % PAGE_SIZE;
        unsafe { self.get_table(address).physical_address() + offset }
    }

    pub fn is_user_page(&self, virtual_address: usize) -> bool
    {
        unsafe { self.get_table(virtual_address) }.is_user_readable()
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
        let tables_address = (&mut *self.directories)[page_directory].physical_address(); // Assumes physical address to tables is also the virtual,
                                                                                          // because for now the kernel is identity mapped

        let tables = &mut *(tables_address as *mut [PageTable; PAGE_TABLES]);

        &mut tables[page_table]
    }

    unsafe fn get_table(&self, virtual_address: usize) -> &PageTable
    {
        let page_directory = virtual_address / DIRECTORY_SIZE;
        let page_table = (virtual_address / PAGE_SIZE) % PAGE_TABLES;
        let tables_address = (&mut *self.directories)[page_directory].physical_address(); // See above
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
