use bitflags::bitflags;

pub type GdtEntry = u64;

bitflags!
{
    pub struct Segment : u16
    {
        const DESCRIPTOR_TYPE = 1 << 0x4; // 0 for system, 1 for code/data
        const PRESENT = 1 << 0x7;
        const SYSTEM_USE = 1 << 0xc;
        const LONG_MODE = 1 << 0xd;
        const IS_32_BIT = 1 << 0xe;
        const GRANULARITY = 1 << 0xf; // 0 for 1B - 1MB, 1 for 4KB - 4KB
        const PRIVILEGE_LEVEL_1 = 1 << 0x5;
        const PRIVILEGE_LEVEL_2 = 2 << 0x5;
        const PRIVILEGE_LEVEL_3 = 3 << 0x5;

        const DATA_READ = 0x0;
        const DATA_READ_ACCESSED = 0x1;
        const DATA_READ_WRITE = 0x2;
        const DATA_READ_WRITE_ACCESSED = 0x3;
        const DATA_READ_WRITE_EXPAND_DOWN = 0x5;
        const DATA_READ_WRITE_EXPAND_DOWN_ACCESSED = 0x7;

        const CODE_EXECUTE_ONLY = 0x8;
        const CODE_EXECUTE_ONLY_ACCESSED = 0x9;
        const CODE_EXECUTE_READ = 0xa;
        const CODE_EXECUTE_READ_ACCESSED = 0xb;
        const CODE_EXECUTE_CONFORMING = 0xc;
        const CODE_EXECUTE_CONFORMING_ACCESSED = 0xd;
        const CODE_EXECUTE_READ_CONFORMING = 0xe;
        const CODE_EXECUTE_READ_CONFORMING_ACCESSED = 0xf;

        const NONE = 0;

        const CODE_RING_0 = Self::DESCRIPTOR_TYPE.bits | Self::PRESENT.bits | Self::IS_32_BIT.bits |
                            Self::GRANULARITY.bits | Self::CODE_EXECUTE_READ.bits;

        const DATA_RING_0 = Self::DESCRIPTOR_TYPE.bits | Self::PRESENT.bits | Self::IS_32_BIT.bits |
                            Self::GRANULARITY.bits | Self::DATA_READ_WRITE.bits;

        const CODE_RING_3 = Self::DESCRIPTOR_TYPE.bits | Self::PRESENT.bits | Self::IS_32_BIT.bits |
                            Self::GRANULARITY.bits | Self::CODE_EXECUTE_READ.bits | Self::PRIVILEGE_LEVEL_3.bits;

        const DATA_RING_3 = Self::DESCRIPTOR_TYPE.bits | Self::PRESENT.bits | Self::IS_32_BIT.bits |
                            Self::GRANULARITY.bits | Self::DATA_READ_WRITE.bits | Self::PRIVILEGE_LEVEL_3.bits;

        const TSS_RING_0 = Self::PRESENT.bits | Self::IS_32_BIT.bits | Self::CODE_EXECUTE_ONLY_ACCESSED.bits;
    }
}

pub fn create_gdt_entry(base: u32, limit: u32, flags: Segment) -> GdtEntry
{
    let mut descriptor: u64 = 0;
    let flag_bits = flags.bits() as u64;

    // Create the high 32 bit segment
    descriptor |= limit as u64          & 0x000f0000; // Bits 19:16
    descriptor |= (flag_bits << 8)      & 0x00f0ff00; // Set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= ((base as u64) >> 16) & 0x000000ff; // Base bits 23:16
    descriptor |= base as u64           & 0xff000000; // Base bits 31:24

    // Fill out the low segment
    descriptor <<= 32;
    descriptor |= (base as u64) << 16;
    descriptor |= limit as u64 & 0x0000ffff;

    descriptor
}