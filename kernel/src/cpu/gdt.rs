use bitflags::bitflags;

bitflags!
{
    #[allow(dead_code)]
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
    }
}

pub struct Flag;

#[allow(dead_code)]
impl Flag
{
    pub fn none() -> Segment { Segment::empty() }

    pub fn code_ring0() -> Segment { Segment:: DESCRIPTOR_TYPE | Segment::PRESENT | Segment::IS_32_BIT |
                                     Segment::GRANULARITY | Segment::CODE_EXECUTE_READ }

    pub fn data_ring0() -> Segment { Segment:: DESCRIPTOR_TYPE | Segment::PRESENT | Segment::IS_32_BIT |
                                     Segment::GRANULARITY | Segment::DATA_READ_WRITE }


    pub fn code_ring3() -> Segment { Segment:: DESCRIPTOR_TYPE | Segment::PRESENT | Segment::IS_32_BIT |
                                     Segment::GRANULARITY | Segment::CODE_EXECUTE_READ | Segment::PRIVILEGE_LEVEL_3 }

    pub fn data_ring3() -> Segment { Segment:: DESCRIPTOR_TYPE | Segment::PRESENT | Segment::IS_32_BIT |
                                     Segment::GRANULARITY | Segment::DATA_READ_WRITE | Segment::PRIVILEGE_LEVEL_3 }
}