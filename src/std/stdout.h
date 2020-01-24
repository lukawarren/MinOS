#ifndef STDOUT_H
#define SDTOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Hardware text mode color constants. */
enum vga_color {
     VGA_COLOR_BLACK = 0,
     VGA_COLOR_BLUE = 1,
     VGA_COLOR_GREEN = 2,
     VGA_COLOR_CYAN = 3,
     VGA_COLOR_RED = 4,
     VGA_COLOR_MAGENTA = 5,
     VGA_COLOR_BROWN = 6,
     VGA_COLOR_LIGHT_GREY = 7,
     VGA_COLOR_DARK_GREY = 8,
     VGA_COLOR_LIGHT_BLUE = 9,
     VGA_COLOR_LIGHT_GREEN = 10,
     VGA_COLOR_LIGHT_CYAN = 11,
     VGA_COLOR_LIGHT_RED = 12,
     VGA_COLOR_LIGHT_MAGENTA = 13,
     VGA_COLOR_LIGHT_BROWN = 14,
     VGA_COLOR_WHITE = 15,
};

size_t stdout_row;
size_t stdout_column;
uint8_t stdout_color;
uint16_t* stdout_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
     return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
     return (uint16_t) uc | (uint16_t) color << 8;
}

void stdout_initialize(void);
void stdout_setcolor(uint8_t color);
void stdout_putentryat(char c, uint8_t color, size_t x, size_t y);
void stdout_putchar(char c);
void std_write(const char* data, size_t size);

void printf(const char* data);
void printn(unsigned int x);
void printh(unsigned int x);
void printi(const char* data);
void printxy(const char* data, int x, int y);


void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);

static const size_t starting_column = 1; // Always nice to have a bit of a margin :-)

#endif
