//
// Created by root on 9/1/23.
//

#include "../../include/linux/kernel.h"
#include "../../include/linux/traps.h"

void clock_handler(int idt_index) {
    send_eoi(idt_index);

    printk("set eoi 0x%x\n", idt_index);
}