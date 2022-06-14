use super::vga_types::*;
use core::ptr;
use core::fmt;

const VGA_WIDTH: usize = 80;
const VGA_HEIGHT: usize = 25;
const VGA_ADDRESS: usize = 0xb8000;
type VgaBuffer = *mut [[VgaCharacter; VGA_WIDTH]; VGA_HEIGHT];

pub struct TextDevice
{
    x: usize,
    y: usize,
    colour: Colour,
    buffer: VgaBuffer
}

impl TextDevice
{
    pub const fn default() -> Self
    {
        TextDevice
        {
            x: 0,
            y: 0,
            colour: Colour::new(ColourCode::White, ColourCode::Black),
            buffer: VGA_ADDRESS as VgaBuffer
        }
    }

    pub fn draw_char(&mut self, character: u8)
    {
        // Wrapping - TODO: scroll
        if self.x >= VGA_WIDTH { self.y += 1; self.x = 0; }
        if self.y >= VGA_HEIGHT { self.y = 0; }

        if character == b'\n' { self.newline(); }
        else
        {
            unsafe
            {
                ptr::write_volatile(&mut (*self.buffer)[self.y][self.x], VgaCharacter
                {
                    character,
                    colour: self.colour
                });
            }
            self.x += 1;
        }
    }

    pub fn draw_string(&mut self, string: &str)
    {
        for char in string.bytes() {
            self.draw_char(char);
        }
    }

    pub fn clear(&mut self)
    {
        self.x = 0;
        self.y = 0;

        for _ in 0..VGA_WIDTH*VGA_HEIGHT {
            self.draw_char(b' ');
        }

        self.x = 0;
        self.y = 0;

    }

    fn newline(&mut self)
    {
        self.x = 0;
        self.y += 1;
    }
}

// For Rust's formatting macros
// ============ vv ============

impl fmt::Write for TextDevice
{
    fn write_str(&mut self, string: &str) -> fmt::Result
    {
        self.draw_string(string);
        Ok(())
    }
}

#[macro_export]
macro_rules! print
{
    ($($arg:tt)*) => ($crate::graphics::vga::_print(format_args!($($arg)*)));
}

#[macro_export]
macro_rules! println
{
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ($crate::print!("{}\n", format_args!($($arg)*)));
}

pub static mut GLOBAL_TEXT_DEVICE: TextDevice = TextDevice::default();

pub fn _print(args: fmt::Arguments)
{
    // TODO: consider synchronisation primitives
    use fmt::Write;
    unsafe { GLOBAL_TEXT_DEVICE.write_fmt(args).unwrap(); }
}