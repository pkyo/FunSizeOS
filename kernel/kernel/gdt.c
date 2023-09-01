//
// Created by root on 8/3/23.
//

#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"
#include "../include/linux/types.h"
#include "../include/string.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpointer-to-int-cast"
#pragma ide diagnostic ignored "ConstantConditionsOC"
#pragma ide diagnostic ignored "ConstantParameter"
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"


xdt_ptr_t gdt_ptr;

#define GDT_SIZE    256

u64 gdt[GDT_SIZE] = {0};

int r3_code_selector;

int r3_data_selector;

static void r3_gdt_code_item(int gdt_index, int base, int limit) {
    // Four global descriptors have already been constructed in real mode.
    if (gdt_index < 4) {
        printk("the gdt index %d has been used ...\n", gdt_index);

        return;
    }

    gdt_item_t *item = (gdt_item_t *) &gdt[gdt_index];

    // The base field is 32 bits. It specifies the starting address of the memory segment.
    // The limit field is 20 bits. It specifies the size of the memory segment.
    item->limit_low = limit & 0xffff;           // Limit (0-15)
    item->base_low = base & 0xffffff;           // Base (0-23)
    item->type = 0b1000;                        // 1000 indicates a code segment that can be executed and read
    item->segment = 1;                          // 1 for code or data segments
    item->DPL = 0b11;                           // Typically represents the lowest privilege level
    item->present = 1;                          // Must be 1
    item->limit_high = limit >> 16 & 0xf;       // Limit (16-19)
    item->available = 0;
    item->long_mode = 0;                        // Determines whether the segment is a 64-bit code segment (L=1) or not (L=0)
    item->big = 1;                              // D/B is the default operation size (1 for 32-bit segment, 0 for 16-bit segment)
    item->granularity = 1;                      // 0 for byte granularity, 1 for 4KB granularity
    item->base_high = base >> 24 & 0xff;        // Base (24-31)
}

static void r3_gdt_data_item(int gdt_index, int base, int limit) {
    if (gdt_index < 4) {
        printk("the gdt index %d has been used ...\n", gdt_index);

        return;
    }

    gdt_item_t *item = (gdt_item_t *) &gdt[gdt_index];

    item->limit_low = limit & 0xffff;
    item->base_low = base & 0xffffff;
    item->type = 0b0010;                        // 0010 indicates a data segment that can be read and written
    item->segment = 1;
    item->DPL = 0b11;
    item->present = 1;
    item->limit_high = limit >> 16 & 0xf;
    item->available = 0;
    item->long_mode = 0;
    item->big = 1;
    item->granularity = 1;
    item->base_high = base >> 24 & 0xff;
}

void gdt_init() {
    // The `volatile` keyword tells the compiler not to optimize this block of code,
    // ensuring that it is executed exactly as written.
    // `sgdt gdt_ptr;` stores the Global Descriptor Table (GDT) register into
    // the memory location pointed to by `gdt_ptr`.
    __asm__ volatile ("sgdt gdt_ptr;");

    memcpy(&gdt, (void *) gdt_ptr.base, (size_t) gdt_ptr.limit);

    r3_gdt_code_item(4, 0, 0xfffff);
    r3_gdt_data_item(5, 0, 0xfffff);

    // 0b011
    // The lowest two bits (bit 0 and bit 1) define the Requested Privilege Level
    // The TI is 0, the selector is describing a segment in the GDT.
    r3_code_selector = 4 << 3 | 0b011;
    r3_data_selector = 5 << 3 | 0b011;

    gdt_ptr.base = (int) &gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;

    // BOCHS_DEBUG_MAGIC

    // Loads the Global Descriptor Table (GDT) register from the memory location pointed to by `gdt_ptr`
    __asm__ volatile ("lgdt gdt_ptr;");
}


#pragma clang diagnostic pop