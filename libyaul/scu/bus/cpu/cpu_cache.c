/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <cpu/cache.h>

void __section(".uncached")
cpu_cache_purge_line(void *addr)
{
        volatile uint32_t *purge_addr;
        purge_addr = (volatile uint32_t *)(CPU_CACHE_PURGE | (uintptr_t)addr);

        *purge_addr = 0x00000000;
}

void __section(".uncached")
cpu_cache_purge(void)
{
        volatile uint8_t * const reg_ccr = (volatile uint8_t * const)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr;

        t0 &= ~0x01;
        *reg_ccr = t0;

        t0 |= 0x10;
        *reg_ccr = t0;

        t0 |= 0x01;
        *reg_ccr = t0;
}
