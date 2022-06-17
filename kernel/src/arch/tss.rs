#[derive(Copy, Clone)]
#[repr(C, packed)]
pub struct TSS
{
    pub previous_tss: u32,  // The previous TSS, forming a linked list
    pub esp0: u32,          // The stack pointer to load for kernel mode
    pub ss0: u32,           // The stack segment to load for kernel mode
    pub esp1: u32,
    pub ss1: u32,
    pub esp2: u32,
    pub ss2: u32,
    pub cr3: u32,
    pub eip: u32,
    pub eflags: u32,
    pub eax: u32,
    pub ecx: u32,
    pub edx: u32,
    pub ebx: u32,
    pub esp: u32,
    pub ebp: u32,
    pub esi: u32,
    pub edi: u32,
    pub es: u32,
    pub cs: u32,
    pub ss: u32,
    pub ds: u32,
    pub fs: u32,
    pub gs: u32,
    pub ldt: u32,
    pub trap: u16,
    pub iomap_base: u16
}

impl TSS
{
    // Can't just derive because it has to be const
    pub const fn default() -> TSS
    {
        TSS
        {
            previous_tss: 0,
            esp0: 0,
            ss0: 0,
            esp1: 0,
            ss1: 0,
            esp2: 0,
            ss2: 0,
            cr3: 0,
            eip: 0,
            eflags: 0,
            eax: 0,
            ecx: 0,
            edx: 0,
            ebx: 0,
            esp: 0,
            ebp: 0,
            esi: 0,
            edi: 0,
            es: 0,
            cs: 0,
            ss: 0,
            ds: 0,
            fs: 0,
            gs: 0,
            ldt: 0,
            trap: 0,
            iomap_base: 0
        }
    }
}