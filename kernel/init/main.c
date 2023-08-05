#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void kernel_main(void) {
    console_init();

    gdt_init();

    idt_init();

    char *s = "Peter.Pei";

    for (int i = 0; i < 30; ++i) {
        printk("My name is %s, and my age is %d\n", s, i);
    }

    // The "sti;" instruction stands for "Set Interrupt Flag".
    // It is an assembly instruction for x86 processors that
    // enables hardware interrupts by setting the interrupt flag in the EFLAGS register.
    // Once this flag is set, the CPU is allowed to process hardware interrupts.
    __asm__("sti;");

    while (true);
}

#pragma clang diagnostic pop