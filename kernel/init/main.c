#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"
#include "../include/linux/mm.h"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

extern void clock_init();

void kernel_main(void) {
    console_init();

    gdt_init();

    idt_init();

    // In the printk function, the STI (Set Interrupt) instruction is executed to enable interrupts.
    // Therefore, it is crucial to use printk only after the IDT (Interrupt Descriptor Table) has been initialized.
    // Otherwise, if an interrupt arrives before the IDT is fully set up, it could lead to issues.
    printk("init gdt success ...\n");
    printk("init idt success ...\n");

    clock_init();

//    char *s = "Peter.Pei";
//
//    for (int i = 0; i < 30; ++i) {
//        printk("My name is %s, and my age is %d\n", s, i);
//    }

    printk("Hello FunsizeOS ...\n");

    print_check_memory_info();
    memory_init();
    memory_map_int();

    // The "sti;" instruction stands for "Set Interrupt Flag".
    // It is an assembly instruction for x86 processors that
    // enables hardware interrupts by setting the interrupt flag in the EFLAGS register.
    // Once this flag is set, the CPU is allowed to process hardware interrupts.
    __asm__("sti;");

    // divide by zero
    // int i = 10 / 0;

    while (true);
}

#pragma clang diagnostic pop