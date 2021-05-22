#pragma once
#ifndef PIC_H
#define PIC_H

#include <stdint.h>
#include <stddef.h>

#define PIC1		    0x20		/* IO base address for master PIC */
#define PIC2		    0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	    (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	    (PIC2+1)

#define PIC_INIT        0x11        /* Makes the PIC wait for 3 extra init messages */
#define PIC_EOI         0x20        /* Ends interrupt */

#define ICW1_ICW4	    0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	    0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08		 /* Level triggered (edge) mode */
#define ICW1_INIT	    0x10		/* Initialization - required! */

#define ICW4_8086	    0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	    0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	    0x10		/* Special fully nested (not) */

#define PIC_MASK_PIT_CODE               0x1
#define PIC_MASK_KEYBOARD_CODE          0x2
#define PIC_MASK_MOUSE_CODE             0xc
#define PIC_MASK_PIT                    0xff ^ PIC_MASK_PIT_CODE
#define PIC_MASK_KEYBOARD               0xff ^ PIC_MASK_KEYBOARD_CODE
#define PIC_MASK_PIT_AND_KEYBOARD       0xff ^ PIC_MASK_PIT_CODE ^ PIC_MASK_KEYBOARD_CODE
#define PIC_MASK_PIT_AND_MOUSE          0xff ^ PIC_MASK_PIT_CODE ^ PIC_MASK_MOUSE_CODE
#define PIC_PIT_KEYBOARD_AND_MOUSE      0xff ^ PIC_MASK_PIT ^ PIC_MASK_KEYBOARD ^ PIC_MASK_MOUSE_CODE
#define PIC_MASK_ALL                    0xff

namespace PIC
{
    void Init(const uint8_t mask1, const uint8_t mask2);
    void EndInterrupt(uint8_t irq);
    void SetMask(uint8_t irq);
    void ClearMask(uint8_t irq);
}

#endif