#include "../serial.h"

#include "stdout.h"
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

//static const size_t starting_column = 1; // Exists in header file

void stdout_initialize(void) 
{
	stdout_row = 0;
	stdout_column = starting_column;
	stdout_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	stdout_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			stdout_buffer[index] = vga_entry(' ', stdout_color);
		}
	}

	enable_cursor(0, 0);

}
 
void stdout_setcolor(uint8_t color) 
{
	stdout_color = color;
}
 
void stdout_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	stdout_buffer[index] = vga_entry(c, color);
}
 
void stdout_putchar(char c)
{
	if (c == '\n')
	{
		++stdout_row;
		stdout_column = starting_column;
		update_cursor(stdout_column, stdout_row);
		return;
	}

	stdout_putentryat(c, stdout_color, stdout_column, stdout_row);
	if (++stdout_column == VGA_WIDTH) {
		stdout_column = starting_column;
		if (++stdout_row >= VGA_HEIGHT)
		{
			stdout_initialize();
			stdout_row = 0;
		}
	}
	update_cursor(stdout_column, stdout_row);
}
 
void stdout_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		stdout_putchar(data[i]);
}
 
void printf(const char* data) 
{
	stdout_write(data, strlen(data));
	stdout_write("\n", strlen("\n"));
}

void printn(unsigned int x)
{
	unsigned int i = x;
	unsigned int dig=1;
 	while (i/=10) dig++;
	int arrDig = dig;
	
	char arr[dig+1];
	while (dig--)
	{
		arr[dig]=x%10 + 48;
		x/=10;
	}
	arr[arrDig] = '\0';
	

	printi(arr);
}

void printh(unsigned int x)
{
	printi("0x");
	unsigned int i = x;
	unsigned int dig=1;
 	while (i/=16) dig++;
	int arrDig = dig;
	
	char arr[dig+1];
	while (dig--)
	{
		int value = x%16 + 48;
		
		if (value > 57) // If ASCII is greater than 9 wrap back down the letters
			value += 7;

		arr[dig] = value;
		x/=16;
	}
	arr[arrDig] = '\0';
	

	printi(arr);
}

void printi(const char* data)
{
	stdout_write(data, strlen(data));
}

void printxy(const char* data, int x, int y)
{
	int last_row = stdout_row;
	int last_col = stdout_column;

	stdout_row = x;
	stdout_column = y;

	for (size_t i = 0; i < strlen(data); i++)
	{
		char c  = data[i];
		if (c == '\n')
		{
			++stdout_row;
			stdout_column = starting_column;
			return;
		}

		stdout_putentryat(c, stdout_color, stdout_column, stdout_row);
		if (++stdout_column == VGA_WIDTH) {
			stdout_column = starting_column;
			if (++stdout_row == VGA_HEIGHT)
				stdout_row = 0;
		}
	}

	stdout_row = last_row;
	stdout_column = last_col;
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void update_cursor(int x, int y)
{
	uint16_t pos = (y+1) * VGA_WIDTH + x;
 	
	if (y >= VGA_HEIGHT)
	{
		stdout_row = 0;
		stdout_initialize();
	}

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}