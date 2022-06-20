use super::paging;
use core::mem::size_of;
use crate::memory::paging::PAGE_SIZE;

const BITS_PER_GROUP: usize = 32;
const NUMBER_OF_GROUPS: usize = paging::MAX_PAGES / BITS_PER_GROUP;

pub struct PageArray
{
    // 0 = allocated, 1 = free
    bit_groups: &'static mut [u32; NUMBER_OF_GROUPS]
}

impl PageArray
{
    pub unsafe fn create_at_address(address: usize) -> PageArray
    {
        let bit_groups = &mut *(address as *mut [u32; NUMBER_OF_GROUPS]);

        // Set everything to allocated and have the caller manually free pages they want
        for i in 0..bit_groups.len() {
            bit_groups[i] = 0;
        }

        PageArray {
            bit_groups
        }
    }

    pub fn allocate_pages(&mut self, pages: usize) -> usize
    {
        if pages == 1
        {
            // Find the first non-zero page group
            for i in 0..NUMBER_OF_GROUPS
            {
                if self.bit_groups[i] != 0
                {
                    let nth_bit = Self::get_position_of_least_significant_bit(self.bit_groups[i]) as usize;
                    let nth_page = i * BITS_PER_GROUP + nth_bit;

                    self.clear_page(nth_page);
                    return nth_page * PAGE_SIZE
                }
            }
        }

        else
        {
            todo!();
        }

        panic!("no free pages found");
    }

    pub fn free_pages(&mut self, address: usize, pages: usize)
    {
        for page in 0..pages
        {
            self.set_page(address / PAGE_SIZE + page);
        }
    }

    pub fn size() -> usize {
        size_of::<u32>() * NUMBER_OF_GROUPS
    }

    fn clear_page(&mut self, page: usize)
    {
        // Clear bit
        let group = page / BITS_PER_GROUP;
        let bit = page % BITS_PER_GROUP;
        self.bit_groups[group] &= !(1 << bit);
    }

    fn set_page(&mut self, page: usize)
    {
        // Set bit
        let group = page / BITS_PER_GROUP;
        let bit = page % BITS_PER_GROUP;
        self.bit_groups[group] |= 1 << bit;
    }

    /*
        https://graphics.stanford.edu/~seander/bithacks.html
        "Count the consecutive zero bits (trailing) on the right with multiply and lookup"

        AND'ing a number with its negative extracts the least significant 1 bit from v. The constant
        0x077cb531 is a de Bruijn sequence, which produces a unique pattern of bits into the high 5
        bits for each possible bit position that it is multiplied against. When there are no bits set,
        it returns 0.
    */
    fn get_position_of_least_significant_bit(number: u32) -> u32
    {
        let positions: [u32; 32] = [
            0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
            31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
        ];

        let number_flipped = number.wrapping_neg();
        let index = ((number & number_flipped).wrapping_mul(0x77cb531u32)) >> 27;
        positions[index as usize]
    }
}