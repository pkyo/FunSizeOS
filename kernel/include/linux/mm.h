//
// Created by root on 9/11/23.
//

#ifndef FUNSIZEOS_MM_H
#define FUNSIZEOS_MM_H

#include "types.h"

#define PAGE_SIZE 4096

/*
When using `INT 0x15, EAX = 0xE820` to query the system address map,
the returned 20 bytes typically represent a Memory Range Descriptor.
Here's a breakdown of this 20-byte structure:
    Base Address (8 bytes):
        BaseAddrLow (4 bytes): Represents the lower 32 bits of the base address for the memory region.
        BaseAddrHigh (4 bytes): Represents the upper 32 bits of the base address for the memory region.
    Length (8 bytes):
        LengthLow (4 bytes): Represents the lower 32 bits of the length of the memory region.
        LengthHigh (4 bytes): Represents the upper 32 bits of the length of the memory region.
    Type (4 bytes):
        Describes the type of the memory region. Common values include:
            1: Available RAM
            2: Reserved RAM that cannot be used by the operating system.
            3: Memory already reclaimed by ACPI.
            4: Memory already in use by ACPI NVS.
*/

typedef struct {
    unsigned int base_addr_low;
    unsigned int base_addr_high;
    unsigned int length_low;
    unsigned int length_high;
    unsigned int type;
} check_memory_item_t;

typedef struct {
    unsigned short times;
    check_memory_item_t *data;
} check_memory_info_t;

void print_check_memory_info();

#endif //FUNSIZEOS_MM_H
