//
// Created by root on 9/11/23.
//

#include "../include/asm/system.h"
#include "../include/linux/mm.h"
#include "../include/linux/kernel.h"
#include "../include/string.h"

#define ARDS_ADDR   0x1100
#define LOW_MEM     0x100000    // Physical memory below 1M is used by the kernel

#define ZONE_VALID 1
#define ZONE_RESERVED 2

#define VALID_MEMORY_FROM 0x100000

void print_check_memory_info() {
    check_memory_info_t *p = (check_memory_info_t *) ARDS_ADDR;
    check_memory_item_t *p_data = (check_memory_item_t *) (ARDS_ADDR + 2);

    unsigned short times = p->times;

    for (int i = 0; i < times; ++i) {
        check_memory_item_t *tmp = p_data + i;

        printk("%x, %x, %x, %x, %d\n",
               tmp->base_addr_high,
               tmp->base_addr_low,
               tmp->length_high,
               tmp->length_low,
               tmp->type);
    }
}

physics_memory_info_t g_physics_memory;
physics_memory_map_t g_physics_memory_map;

void memory_init() {
    check_memory_info_t *p = (check_memory_info_t *) ARDS_ADDR;
    check_memory_item_t *p_data = (check_memory_item_t *) (ARDS_ADDR + 2);

    for (int i = 0; i < p->times; ++i) {
        check_memory_item_t *tmp = p_data + i;

        if (tmp->base_addr_low > 0 && tmp->type == ZONE_VALID) {
            g_physics_memory.addr_start = tmp->base_addr_low;
            g_physics_memory.valid_mem_size = tmp->length_low;
            g_physics_memory.addr_end = tmp->base_addr_low + tmp->length_low;

            if (VALID_MEMORY_FROM == g_physics_memory.addr_start) {
                break;
            }
        }
    }

    if (VALID_MEMORY_FROM != g_physics_memory.addr_start) {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);

        return;
    }

    // This operation effectively divides the address by 4096 (2^12),
    // assuming that each page is 4096 bytes (or 4 KB) in size.
    g_physics_memory.pages_total = g_physics_memory.addr_end >> 12;
    g_physics_memory.pages_used = 0;
    g_physics_memory.pages_free = g_physics_memory.pages_total - g_physics_memory.pages_used;
}

void memory_map_int() {
    if (VALID_MEMORY_FROM != g_physics_memory.addr_start) {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);

        return;
    }

    g_physics_memory_map.addr_base = (uint) VALID_MEMORY_FROM;
    g_physics_memory_map.map = (uchar *) VALID_MEMORY_FROM;
    g_physics_memory_map.pages_total = g_physics_memory.pages_total;

    memset(g_physics_memory_map.map, 0, g_physics_memory_map.pages_total);

    // In the provided code, each byte (1B) of the bitmap represents a page,
    // so you'd need `g_physics_memory_map.pages_total` bytes to provide a byte-mapping for each page.
    // However, there's a nuance.
    // This bitmap itself will occupy some physical memory.
    // Hence, the need arises to determine how many pages of physical memory the bitmap will consume.
    g_physics_memory_map.bitmap_item_used = g_physics_memory_map.pages_total / PAGE_SIZE;
    if (0 != g_physics_memory_map.pages_total % PAGE_SIZE) {
        g_physics_memory_map.bitmap_item_used += 1;
    }

    for (int i = 0; i < g_physics_memory_map.bitmap_item_used; ++i) {
        g_physics_memory_map.map[i] = 1;
    }

    printk("physical memory map position: 0x%X(%dM) - 0x%X(%dM)\n",
           g_physics_memory_map.map,
           ((int) g_physics_memory_map.map) / 1024 / 1024,
           g_physics_memory_map.addr_base,
           g_physics_memory_map.addr_base / 1024 / 1024);

    printk("physical memory starts here: 0x%X(%dM), used: %d pages\n",
           g_physics_memory_map.addr_base,
           g_physics_memory_map.addr_base / 1024 / 1024,
           g_physics_memory_map.bitmap_item_used);
}