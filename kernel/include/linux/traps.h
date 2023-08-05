//
// Created by root on 8/3/23.
//

#ifndef FUNSIZEOS_TRAPS_H
#define FUNSIZEOS_TRAPS_H

#include "head.h"

void gdt_init();

void idt_init();

void send_eoi(int idt_index);

#endif //FUNSIZEOS_TRAPS_H
