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

void __uncached_function
cpu_cache_line_purge(void *address)
{
        volatile uint32_t * const purge_address =
            (volatile uint32_t *)CALCULATE_PURGE_ADDRESS(address);

        *purge_address = 0x00000000;
}

void __uncached_function
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

void __no_reorder __uncached_function
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

void __uncached_function
cpu_cache_data_way_read(uint8_t way, cpu_cache_data_way_t *data_way)
{
        volatile uint8_t * const reg_ccr = (volatile uint8_t * const)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr;

        /* Specify which way to read from */
        t0 &= ~0xC0;
        t0 |= (way & 3) << 6;

        *reg_ccr = t0;

        volatile uint32_t *cache_data_rw;
        cache_data_rw = (volatile uint32_t *)CPU_CACHE_ADDRESS_RW;

        /* Force uncache writes in case the user forgets */
        volatile uint32_t *uncached_cache_way;
        uncached_cache_way = (volatile uint32_t *)(CPU_CACHE_THROUGH | (uintptr_t)data_way->data);

        const uint32_t count = CPU_CACHE_WAY_SIZE / CPU_CACHE_LINE_SIZE;

        for (uint32_t i = 0; i < count; i++) {
                *uncached_cache_way = *cache_data_rw;

                uncached_cache_way++;
                /* Skip the rest of the cache line, as it's the same data
                 * repeated 4 times */
                cache_data_rw += 4;
        }
}
