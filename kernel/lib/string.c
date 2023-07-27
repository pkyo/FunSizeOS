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