#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void kernel_main(void) {
    console_init();

    gdt_init();

    char *s = "Peter.Pei";

    for (int i = 0; i < 30; ++i) {
        printk("My name is %s, and my age is %d\n", s, i);
    }

    while (true);
}

#pragma clang diagnostic pop