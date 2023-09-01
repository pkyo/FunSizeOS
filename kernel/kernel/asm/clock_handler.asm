[bits 32]
[SECTION .text]

extern printk
extern clock_handler

global clock_handler_entry
clock_handler_entry:
    push 0x20 ; clock interrupt
    ; To the Programmable Interrupt Controller (PIC) to indicate that the interrupt has been acknowledged.
    call clock_handler
    add esp, 4

    iret

msg:
    db "clock handler", 10, 0