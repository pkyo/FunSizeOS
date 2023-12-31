//
// Created by root on 7/27/23.
//

#ifndef FUNSIZEOS_STRING_H
#define FUNSIZEOS_STRING_H

#include "asm/system.h"

char *strcpy(char *dest, const char *src);

size_t strlen(const char *str);

void *memcpy(void *dest, const void *src, size_t count);

void *memset(void *dest, int ch, size_t count);

#endif //FUNSIZEOS_STRING_H
