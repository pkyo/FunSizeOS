//
// Created by root on 7/10/23.
//

#ifndef FUNSIZEOS_STDARG_H
#define FUNSIZEOS_STDARG_H

// A standard header file in C programming that provides facilities for working with variable argument lists.

// Defines the `va_list` type as `char*`
typedef char *va_list;

// This increments the pointer by the size of a `char*`,
// positioning it after the last named argument and at the beginning of the variable argument list
#define va_start(p, named_arg)      (p = (va_list)&named_arg + sizeof(char*))

// In an X86 32-bit system, the stack typically stores data in units of 4 bytes
// Each data unit, often referred to as a "slot" in the stack has a size of 4 bytes, also known as a "double word"
// In a 32-bit system, `sizeof(char*)` is equal to 4 bytes.
// This is because in a 32-bit system, the size of a pointer is typically matched to the size of registers,
// which is 4 bytes in a 32-bit system

// p is the pointer to the current argument,
// and t is the type of the argument to be retrieved
// `(p += sizeof(char*))` moving it to the next argument position
// `*(t*)` casts the resulting pointer back to the desired type `t*`
// The final `*` dereferences the pointer, obtaining the value of type t from the current argument position
#define va_arg(p, t)            (*(t*)((p += sizeof(char*)) - sizeof(char*)))

#define va_end(p)               (p = 0)

#endif //FUNSIZEOS_STDARG_H
