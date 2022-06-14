#[derive(Copy, Clone)]
#[allow(dead_code)]
#[repr(u8)]
pub enum ColourCode
{
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    Pink = 13,
    Yellow = 14,
    White = 15,
}

#[derive(Copy, Clone)]
#[repr(transparent)]
pub struct Colour(u8);
impl Colour
{
    pub const fn new(foreground: ColourCode, background: ColourCode) ->  Colour
    {
        Colour((background as u8) << 4 | (foreground as u8))
    }
}

#[repr(C)]
pub struct VgaCharacter
{
    pub character: u8,
    pub colour: Colour
}