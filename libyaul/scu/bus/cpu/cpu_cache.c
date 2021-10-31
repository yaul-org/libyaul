/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <math.h>

#include <cpu/cache.h>

#define CALCULATE_PURGE_ADDRESS(x)                                             \
    (CPU_CACHE_PURGE | ((uintptr_t)(x) & ~(CPU_CACHE_LINE_SIZE - 1)))

void __section(".uncached")
cpu_cache_line_purge(void *address)
{
        volatile uint32_t * const purge_address =
            (volatile uint32_t *)CALCULATE_PURGE_ADDRESS(address);

        *purge_address = 0x00000000;
}

void __section(".uncached")
cpu_cache_area_purge(void *address, uint32_t len)
{
        const uint32_t cache_line_count =
            uint32_pow2_round(len, 4) / CPU_CACHE_LINE_SIZE;

        volatile uint32_t *purge_address;
        purge_address = (volatile uint32_t *)CALCULATE_PURGE_ADDRESS(address);

        for (uint32_t line = 0; line < cache_line_count; line++) {
                *purge_address = 0x00000000;

                purge_address += CPU_CACHE_LINE_SIZE / sizeof(uint32_t);
        }
}

void __no_reorder __section(".uncached")
cpu_cache_purge(void)
{
        volatile uint8_t * const reg_ccr = (volatile uint8_t * const)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr;

        t0 &= ~0x01;
        *reg_ccr = t0; /* Disable cache */
        t0 |= 0x10;
        *reg_ccr = t0; /* Purge cache */
        t0 |= 0x01;
        *reg_ccr = t0; /* Enable cache */
}
