//
// Created by root on 7/27/23.
//

#include "../include/string.h"


size_t strlen(const char *str) {
    char *ptr = (char *) str;

    while (*ptr != EOS) {
        ptr++;
    }

    return ptr - str;
}

void *memcpy(void *dest, const void *src, size_t count) {
    char *ptr = dest;

    while (count--) {
        *ptr++ = *((char *) (src++));
    }

    return dest;
}

void *memset(void *dest, int ch, size_t count) {
    char *ptr = dest;

    while (count--) {
        *ptr++ = ch;
    }

    return dest;
}

char *strcpy(char *dest, const char *src) {
    char *ptr = dest;

    while (true) {
        *ptr++ = *src;

        if (*src++ == EOS)
            return dest;
    }
}