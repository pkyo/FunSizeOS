//
// Created by root on 9/14/23.
//


#include "../include/asm/system.h"
#include "../include/linux/kernel.h"
#include "../include/linux/mm.h"
#include "../include/string.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
#pragma clang diagnostic ignored "-Wvoid-pointer-to-int-cast"
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"
#pragma clang diagnostic ignored "-Wpointer-to-int-cast"

#define PDT_START_ADDR 0x20000

void *virtual_memory_init() {
    // A page directory table (pdt) is allocated
    int *pdt = (int *) PDT_START_ADDR;

    memset(pdt, 0, PAGE_SIZE);

    // Loop to Set Up the First 4 Page Tables
    for (int i = 0; i < 4; ++i) {
        // A page table (ptt) is allocated
        int ptt = (int) PDT_START_ADDR + ((i + 1) * 0x1000);
        // Page Directory Entry, PDE
        // | 31-12       | 11-9 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
        // |:-----------:|:----:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
        // | Base Address| Avail|  G  |  0  |  D  |  A  | PCD | PWT | U/S | R/W |  P  |
        // When you acquire a new page from the `get_free_page()` function,
        // the address you receive will be 4KB-aligned, implying its lowest 12 bits are zeros.
        int pde = 0b00000000000000000000000000000111 | ptt;

        pdt[i] = pde; // 4B

        int *ptt_arr = (int *) ptt;

        if (0 == i) {
            // 0x400 -> 1024
            for (int j = 0; j < 0x400; ++j) {
                int *item = &ptt_arr[j];

                // 0x1000 -> 4096
                // virtual_addr lowest 12 bits are zeros
                int virtual_addr = j * 0x1000;

                *item = 0b00000000000000000000000000000111 | virtual_addr;
            }
        } else {
            for (int j = 0; j < 0x400; ++j) {
                int *item = &ptt_arr[j];

                int virtual_addr = j * 0x1000;

                // 0x400 * 0x1000 -> 1024 * 4096
                // virtual_addr lowest 12 bits are zeros
                virtual_addr = virtual_addr + i * 0x400 * 0x1000;

                *item = 0b00000000000000000000000000000111 | virtual_addr;
            }
        }
    }

    BOCHS_DEBUG_MAGIC

    set_cr3((uint) pdt);

    enable_page();

    BOCHS_DEBUG_MAGIC

    return pdt;
}

#pragma clang diagnostic pop