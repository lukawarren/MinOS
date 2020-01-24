#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

#include <stdint.h>

/*

   0	Programmable Interrupt Timer Interrupt
   1	Keyboard Interrupt
   2	Cascade (used internally by the two PICs. never raised)
   3	COM2 (if enabled)
   4	COM1 (if enabled)
   5	LPT2 (if enabled)
   6	Floppy Disk
   7	LPT1 / Unreliable "spurious" interrupt (usually)
   8	CMOS real-time clock (if enabled)
   9	Free for peripherals / legacy SCSI / NIC
   10	Free for peripherals / SCSI / NIC
   11	Free for peripherals / SCSI / NIC
   12	PS2 Mouse
   13	FPU / Coprocessor / Inter-processor
   14	Primary ATA Hard Disk
   15	Secondary ATA Hard Disk

 */

/* ------------------------------------------------------------- */

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

/* ------------------------------------------------------------- */

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */

typedef struct IDTEntry
{
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t zero;      // unused, set to 0
   uint8_t type_attr; // type and attributes, see below
   uint16_t offset_2; // offset bits 16..31
} __attribute__((packed)) IDTEntry;

typedef struct IDTPointer
{
   uint16_t limit; // Size of the IDT
   uint32_t base; // Base address of the IDT
} __attribute__((packed)) IDTPointer;

void io_wait();
void initIDT();

void blankIQR(uint8_t iqr);

void endIQR(uint8_t iqr);

void iretf();

void setIDT(uint32_t IDT);

#endif