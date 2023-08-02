#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void kernel_main(void) {
    console_init();

    char *s = "Peter.Pei";

    for (int i = 0; i < 30; ++i) {
        printk("my name is %s, and my age is %d\n", s, i);
    }

    while (true);
}

#pragma clang diagnostic pop