//
// Created by root on 8/5/23.
//

#include "../include/asm/io.h"
#include "../include/linux/traps.h"

#define PIC_M_CTRL  0x20        // The control port of the master PIC.
#define PIC_M_DATA  0x21        // The data port of the master PIC.
#define PIC_S_CTRL  0xa0        // The control port of the slave PIC.
#define PIC_S_DATA  0xa1        //  The data port of the slave PIC.
// This is the signal sent to the PIC to tell it that the current interrupt has been dealt with
// and the PIC can now issue the next interrupt.
#define PIC_EOI     0x20        // The End of Interrupt (EOI) command code as 0x20.

void send_eoi(int idt_index) {
    if (idt_index >= 0x20 && idt_index < 0x28) {
        // If the IDT index is between 0x20 and 0x27 (inclusive),
        // it corresponds to an interrupt handled by the master PIC.
        out_byte(PIC_M_CTRL, PIC_EOI);

    } else if (idt_index >= 0x28 && idt_index < 0x30) {
        // If the IDT index is between 0x28 and 0x2F (inclusive),
        // it corresponds to an interrupt handled by the slave PIC.
        // In this case, an EOI signal is first sent to the master PIC,
        // then another EOI signal is sent to the slave PIC.
        out_byte(PIC_M_CTRL, PIC_EOI);
        out_byte(PIC_S_CTRL, PIC_EOI);
    }
}

void write_xdt_ptr(xdt_ptr_t *p, short limit, int base) {
    p->limit = limit;

    p->base = base;
}