[bits 32]
[SECTION .text]

; Declares that printk is an external function, meaning it is defined somewhere else, likely in the C code.
extern printk

extern keymap_handler

; As a global symbol, it can be accessed from other files.
global interrupt_handler
interrupt_handler:
    push msg

    call printk

    ; Cleans up the stack by adding 4 to the stack pointer esp.
    ; This effectively removes the previously pushed address from the stack.
    add esp, 4

    ; It pops the state of the CPU registers off the stack,
    ; which were saved when the interrupt occurred,
    ; restoring the state of the CPU to what it was before the interrupt.
    iret

global keymap_handler_entry
keymap_handler_entry:
    push 0x21
    call keymap_handler
    ; Add 4 to the stack pointer, effectively removing the pushed value from the stack
    add esp, 4

    iret


msg:
    db "interrupt handler ...", 10, 0