use bitflags::bitflags;

bitflags!
{
    pub struct Attributes: u8
    {
        const ENABLED = 1 << 7;
        const MIN_CALLER_PRIVILEGE_RING_0 = 0 << 5;
        const MIN_CALLER_PRIVILEGE_RING_1 = 1 << 5;
        const MIN_CALLER_PRIVILEGE_RING_2 = 2 << 5;
        const MIN_CALLER_PRIVILEGE_RING_3 = 3 << 5;
        const GATE_INTERRUPT_32 = 0xe;
        const GATE_TRAP_32 = 0xf;

        const ENABLED_RING_0_INTERRUPT  = Self::ENABLED.bits | Self::MIN_CALLER_PRIVILEGE_RING_0.bits | Self::GATE_INTERRUPT_32.bits;
        const ENABLED_RING_3_INTERRUPT  = Self::ENABLED.bits | Self::MIN_CALLER_PRIVILEGE_RING_3.bits | Self::GATE_INTERRUPT_32.bits;
        const DISABLED_RING_0_INTERRUPT = Self::MIN_CALLER_PRIVILEGE_RING_0.bits | Self::GATE_INTERRUPT_32.bits;
        const DISABLED_RING_3_INTERRUPT = Self::MIN_CALLER_PRIVILEGE_RING_3.bits | Self::GATE_INTERRUPT_32.bits;
    }
}

#[derive(Copy, Clone)]
#[repr(C, packed)]
pub struct Entry
{
    pub isr_address_low: u16,   // Lower 16 bits of ISR address
    pub segment_selector: u16,
    pub reserved: u8,           // Always zero
    pub attributes: u8,
    pub isr_address_high: u16   // Upper 16 bits of ISR address
}

impl Entry
{
    pub fn new(function: unsafe extern "C" fn(), selector: u16, attributes: Attributes) -> Entry
    {
        let address = function as *const ()as u32;

        Entry
        {
            isr_address_low: (address & 0xffff) as u16,
            segment_selector: selector,
            reserved: 0,
            attributes: attributes.bits(),
            isr_address_high: ((address & 0xffff0000) >> 16) as u16
        }
    }

    // Has to be done manually so as to be const
    pub const fn default() -> Entry
    {
        Entry
        {
            isr_address_low: 0,
            segment_selector: 0,
            reserved: 0,
            attributes: 0,
            isr_address_high: 0
        }
    }
}

#[derive(Copy, Clone)]
#[repr(C, packed)]
pub struct Table // The actual IDT
{
    pub entries: [Entry; 256]
}

impl Table
{
    pub const fn default() -> Table
    {
        Table {
            entries: [Entry::default(); 256]
        }
    }
}