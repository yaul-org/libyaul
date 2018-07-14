/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_CACHE_H_
#define _CPU_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpu/map.h>

#define CPU_CACHE               0x00000000
#define CPU_CACHE_THROUGH       0x20000000
#define CPU_CACHE_PURGE         0x40000000
#define CPU_CACHE_ADDRESS_RW    0x60000000
#define CPU_CACHE_DATA_RW       0xC0000000
#define CPU_CACHE_IO            0xF0000000

#define CPU_CACHE_WAY_0_ADDR    0xC0000000
#define CPU_CACHE_WAY_1_ADDR    0xC0000400
#define CPU_CACHE_WAY_2_ADDR    0xC0000800
#define CPU_CACHE_WAY_3_ADDR    0xC0000C00

#define CPU_CACHE_2_WAY_SIZE    (CPU_CACHE_WAY_2_ADDR - CPU_CACHE_WAY_0_ADDR)

#define CPU_CACHE_MODE_4_WAY    0x00
#define CPU_CACHE_MODE_2_WAY    0x08

static inline void __attribute__ ((always_inline))
cpu_cache_enable(void)
{
        MEMORY_WRITE_OR(8, CPU(CCR), 0x01);
}

static inline void __attribute__ ((always_inline))
cpu_cache_disable(void)
{
        MEMORY_WRITE_AND(8, CPU(CCR), ~0x01);
}

static inline void __attribute__ ((always_inline))
cpu_cache_way_mode_set(uint8_t mode)
{
        volatile uint8_t *reg_ccr;
        reg_ccr = (uint8_t *)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr & ~0x09;

        *reg_ccr = t0;
        *reg_ccr = t0 | mode;
        *reg_ccr = t0 | mode | 0x01;
}

static inline void __attribute__ ((always_inline))
cpu_cache_purge_line(void *addr)
{
        uint32_t *purge_addr;
        purge_addr = (uint32_t *)(CPU_CACHE_PURGE | (uintptr_t)addr);

        *purge_addr = 0x00000000;
}

static inline void __attribute__ ((always_inline))
cpu_cache_purge(void)
{
        volatile uint8_t *reg_ccr;
        reg_ccr = (uint8_t *)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr & ~0x01;

        *reg_ccr = t0;
        *reg_ccr = t0 | 0x10;
        *reg_ccr = t0 | 0x11;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_CACHE_H */
