//
// Created by root on 8/3/23.
//

#ifndef FUNSIZEOS_HEAD_H
#define FUNSIZEOS_HEAD_H

typedef struct gdt_item_t {
    unsigned short limit_low;               // Limit (0-15)
    unsigned int base_low: 24;              // Base (0-23)
    // P_DPL_S_TYPE
    // TYPE defines the exact type of the segment.
    //      1000 indicates a code segment that can be executed and read
    //      0010 indicates a data segment that can be read and written
    unsigned char type: 4;
    unsigned char segment: 1;               // S is the descriptor type (1 for code or data segments)
    unsigned char DPL: 2;                   // DPL is the descriptor privilege level (00 for kernel mode)
    unsigned char present: 1;               // P is the segment-present flag (must be 1)
    unsigned char limit_high: 4;            // Limit (16-19)
    unsigned char available: 1;             // Available for system use (AVL): Can be used by system software.
    unsigned char long_mode: 1;             // L (64-bit code segment): Determines whether the segment is a 64-bit code segment (L=1) or not (L=0)
    unsigned char big: 1;                   // D/B is the default operation size (1 for 32-bit segment, 0 for 16-bit segment)
    unsigned char granularity: 1;           // G is the granularity flag (0 for byte granularity, 1 for 4KB granularity)
    unsigned char base_high;                // Base (24-31)
} __attribute__((packed)) gdt_item_t;

typedef struct gdt_selector_t {
    char RPL: 2;                            // Requested Privilege Level (RPL): The lowest two bits (bit 0 and bit 1) define the Requested Privilege Level
    char TI: 1;                             // Table Indicator (TI): The second bit (bit 2) is the Table Indicator. If the TI is 0, the selector is describing a segment in the GDT.
    short index: 13;                        // Index: The top 13 bits (bit 3 to bit 15) are the index
} __attribute__((packed)) gdt_selector_t;

#pragma pack(2)
typedef struct xdt_ptr_t {
    short limit;
    int base;
} xdt_ptr_t;
#pragma pack()

#endif //FUNSIZEOS_HEAD_H
