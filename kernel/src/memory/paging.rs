use bitflags::bitflags;
use core::mem::size_of;
use crate::arch;

pub const PAGE_SIZE: usize = 4096;
pub const PAGE_TABLES: usize = 1024;
pub const PAGE_DIRECTORIES: usize = 1024;

bitflags!
{
    struct PageFlags : usize
    {
        const PRESENT = 1;
        const READ_WRITE = 1 << 1;
        const GLOBAL_ACCESS = 1 << 2;

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
    fn new(physical_address: usize) -> PageTable
    {
        assert!(is_page_aligned(physical_address));

        // Would set to be as restrictive as possible by default,
        // (i.e. disabled), but free pages are stored in a linked
        // list... within the pages themselves, so they have to be
        // present.
        PageTable {
            physical_address_with_flags: physical_address |
                PageFlags::KERNEL_PAGE_READ_WRITE.bits()
        }
    }

    fn set(&mut self, physical_address: usize, flags: PageFlags)
    {
        assert!(is_page_aligned(physical_address));
        self.physical_address_with_flags = physical_address | flags.bits();
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
    fn new(physical_table_address: usize) -> PageDirectory
    {
        // Set the page directory to be user-space-readable and
        // present, knowing that the page tables themselves are
        // non-present and restricted to the kernel (by default).
        PageDirectory {
            physical_address_with_flags: physical_table_address |
                PageFlags::USER_PAGE_READ_WRITE.bits()
        }
    }
}

#[allow(dead_code)]
#[repr(packed)]
pub struct PageFrame
{
    directories: &'static mut[PageDirectory; PAGE_DIRECTORIES],
    tables: &'static mut[PageTable; PAGE_TABLES * PAGE_DIRECTORIES]
}

impl PageFrame
{
    pub unsafe fn new(physical_start_address: usize) -> PageFrame
    {
        let directories = &mut *(physical_start_address as *mut [PageDirectory; PAGE_DIRECTORIES]);
        let tables = &mut *((physical_start_address + size_of::<PageDirectory>() * PAGE_DIRECTORIES) as *mut [PageTable; PAGE_TABLES * PAGE_DIRECTORIES]);

        // Identify map directories
        for i in 0..directories.len() {
            directories[i] = PageDirectory::new((&tables[i * PAGE_TABLES] as *const _) as usize);
        }

        // Identify map tables
        for i in 0..tables.len() {
            tables[i] = PageTable::new(i * PAGE_SIZE);
        }

        PageFrame {
            directories,
            tables
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

        self.tables[virtual_address / PAGE_SIZE].set(physical_address, flags);
        arch::flush_tlb();
    }

    pub unsafe fn unmap_page(&mut self, virtual_address: usize)
    {
        assert!(is_page_aligned(virtual_address));

        // Revert back to default identity mapping
        self.tables[virtual_address / PAGE_SIZE] = PageTable::new(virtual_address);
        arch::flush_tlb();
    }

    pub unsafe fn load_to_cpu(&self, physical_start_address: usize)
    {
        arch::cpu::load_cr3(physical_start_address);
        arch::enable_paging();
    }

    pub fn virtual_address_to_physical(&self, address: usize) -> usize
    {
        let offset = address % PAGE_SIZE;
        self.tables[address / PAGE_SIZE].physical_address() + offset
    }

    pub fn size() -> usize
    {
        size_of::<PageDirectory>() * PAGE_DIRECTORIES +
        size_of::<PageTable>() * PAGE_TABLES * PAGE_DIRECTORIES
    }
}

pub fn is_page_aligned(size: usize) -> bool
{
    size % PAGE_SIZE == 0
}