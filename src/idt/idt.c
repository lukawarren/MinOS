#include "idt.h"

#include "../serial.h"
#include "../std/stdout.h"
#include "../keyboard/keyboard.h"

void io_wait()
{
    int x = 0;
    for (int i = 0; i < 9999; i++)
        x += i*3;
}

void initIDT()
{
    int offset1 = 0;
    int offset2 = 0;
    unsigned char a1, a2;
 
    a1 = inb(PIC1_DATA);                        // save masks
    a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
    
    // Masks
    a1 = 0xfd; // Just keyboard interrupts
    a2 = 0xff;

	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);

    // Allocate space for IDT
	IDTEntry IDT[256];

	// Set default ISRs
	for (int i = 0; i < 48; i++)
	{
		uint32_t ISRAddress = (uint32_t) &blankIQR;
		IDT[i].offset_1 = (uint16_t)(ISRAddress & 0xFFFF);
		IDT[i].selector = 0x08; // Code selector
		IDT[i].zero = 0;
		IDT[i].type_attr = 0x8e; // 0b10001110
		IDT[i].offset_2 = (uint16_t)((ISRAddress & 0xFFFF0000) >> 16);
	}
	
	uint32_t ISRAddress = (uint32_t) &keyboardInterrupt;
	IDT[1].offset_1 = (uint16_t)(ISRAddress & 0xFFFF);
	IDT[1].selector = 0x08; // Code selector
	IDT[1].zero = 0;
	IDT[1].type_attr = 0x8e; // 0b10001110
	IDT[1].offset_2 = (uint16_t)((ISRAddress & 0xFFFF0000) >> 16);
	
    printi("Loading IDT at base address ");
    printh((uint32_t) &IDT);
    printi(" with size ");
    printh((uint32_t) sizeof(IDT));
    printf("");

	// Fill the IDT descriptor and submit IDT
	IDTPointer table_ptr;
	table_ptr.limit = sizeof(IDTEntry) * 256;
	table_ptr.base = (uint32_t)&IDT;
	setIDT((uint32_t) &table_ptr);
    
}

void blankIQR(uint8_t iqr)
{
    printf("blank IQR");
    
    endIQR(iqr);
}

void endIQR(uint8_t iqr)
{
    if (iqr >= 8) outb(PIC2_COMMAND, PIC_EOI); // It's a PIC2
    // PIC1 EOI must be called anyway
    outb(PIC1_COMMAND, PIC_EOI);
    iretf();
}