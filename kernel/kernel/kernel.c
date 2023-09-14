//
// Created by root on 9/14/23.
//

#include "../include/linux/kernel.h"

// The `cr3` register is typically used to store the physical address of the page directory used in virtual memory paging.
inline uint get_cr3() {
    asm volatile("mov eax, cr3;");

    // On 32-bit x86 architectures,
    // the `eax` register is conventionally used to store the return value
    // of a function when returning an integer or pointer.
}

inline void set_cr3(uint v) {
    // `::"a"(v)`: This syntax binds the C variable v to the eax register
    // (a is a shorthand for eax in GCC-style inline assembly).
    asm volatile("mov cr3, eax;"::"a"(v));
}

// On an x86 system, the cr0 control register is used to control various aspects of the CPU's operation.
// Bit 31, when set to 1, enables paging.
inline void enable_page() {
    asm volatile("mov eax, cr0;"
                 "or eax, 0x80000000;" // Sets the 31st bit of eax to 1, without changing the other bits. This operation turns on the paging bit.
                 "mov cr0, eax;");
}