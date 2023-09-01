//
// Created by root on 7/27/23.
//

#ifndef FUNSIZEOS_SYSTEM_H
#define FUNSIZEOS_SYSTEM_H

#include "../linux/types.h"

#define BOCHS_DEBUG_MAGIC   __asm__("xchg bx, bx");

#define STI   __asm__("sti");

#define CLI   __asm__("cli");

#endif //FUNSIZEOS_SYSTEM_H
