#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
//
// Created by root on 8/4/23.
//


#include "../include/linux/head.h"
#include "../include/linux/traps.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
#pragma clang diagnostic ignored "-Wpointer-to-int-cast"


#define INTERRUPT_TABLE_SIZE    256

interrupt_gate_t interrupt_table[INTERRUPT_TABLE_SIZE] = {0};

//char idt_ptr[6] = {0};
xdt_ptr_t idt_ptr;

extern void interrupt_handler_entry();

extern void keymap_handler_entry();

extern void clock_handler_entry();

extern int interrupt_handler_table[0x2f];

void idt_init() {
    for (int i = 0; i < INTERRUPT_TABLE_SIZE; ++i) {
        interrupt_gate_t *p = &interrupt_table[i];

        // 🔥 When handling a keyboard interrupt,
        // you typically need to read the scancode from the keyboard controller
        // and translate it to the corresponding key.
        // If you're not correctly reading and handling the scancode,
        // the keyboard controller might stop sending new interrupts.

        // Gets the address of the interrupt handler function.
        int handler = (int) interrupt_handler_entry;

        if (i <= 0x15) {
            handler = (int) interrupt_handler_table[i];
        }

        if (0x20 == i) {
            handler = (int) clock_handler_entry;
        }

        if (0x21 == i) {
            handler = (int) keymap_handler_entry;
        }

        p->offset0 = handler & 0xffff;
        p->offset1 = (handler >> 16) & 0xffff;

        p->selector = 1 << 3;                   // Code segment
        p->reserved = 0;                        // This byte is reserved
        p->type = 0b1110;                       // `0b1110` indicating that this is an interrupt gate
        p->segment = 0;                         // Indicating that this gate belongs to a system segment
        p->DPL = 0;                             // This line sets the DPL (Descriptor Privilege Level) field of the interrupt gate to 0, indicating that this gate can be accessed at kernel level.
        p->present = 1;                         // Indicating that this gate descriptor is valid and present
    }

    // This line is setting the first 2 bytes (short) of idt_ptr to the size of the IDT.
    // The IDT size is calculated as `INTERRUPT_TABLE_SIZE * 8` because each entry in the IDT is 8 bytes long.
    // *(short *) idt_ptr = INTERRUPT_TABLE_SIZE * 8;

    // This line is setting the next 4 bytes (int) of `idt_ptr` (starting from byte 2) to
    // the base address of `interrupt_table`.
    // This makes `idt_ptr` effectively a structure of two fields:
    // a 16-bit limit (size of IDT) and a 32-bit base address (location of IDT).
    // *(int *) (idt_ptr + 2) = (int) interrupt_table;

    write_xdt_ptr(&idt_ptr, INTERRUPT_TABLE_SIZE * 8, (int) interrupt_table);


    // BOCHS_DEBUG_MAGIC

    // Loads the IDT pointer into the CPU.
    asm volatile("lidt idt_ptr;");
}


#pragma clang diagnostic pop
#pragma clang diagnostic pop