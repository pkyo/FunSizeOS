//
// Created by root on 8/2/23.
//

#ifndef FUNSIZEOS_KERNEL_H
#define FUNSIZEOS_KERNEL_H

#include "../stdarg.h"

int vsprintf(char *buf, const char *fmt, va_list args);

int printk(const char *fmt, ...);

#endif //FUNSIZEOS_KERNEL_H
