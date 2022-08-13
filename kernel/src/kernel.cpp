#include "io/uart.h"

extern "C" { void kmain(void); }

void kmain(void) 
{
    uart::init();
    uart::write_string("Hello world!\n3 * 3 = ");
    uart::write_number(3*3);
    while(1) {}
}