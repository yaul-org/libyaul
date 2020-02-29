/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_CACHE_H_
#define _CPU_CACHE_H_

#include <sys/cdefs.h>

#include <cpu/map.h>

__BEGIN_DECLS

/// @defgroup CPU_CACHE_DEFINES
/// @defgroup CPU_CACHE_INLINE_FUNCTIONS
/// @defgroup CPU_CACHE_FUNCTIONS

/// @addtogroup CPU_CACHE_DEFINES
/// @{

/// Not yet documented.
#define CPU_CACHE               0x00000000
/// Not yet documented.
#define CPU_CACHE_THROUGH       0x20000000
/// Not yet documented.
#define CPU_CACHE_PURGE         0x40000000
/// Not yet documented.
#define CPU_CACHE_ADDRESS_RW    0x60000000
/// Not yet documented.
#define CPU_CACHE_DATA_RW       0xC0000000
/// Not yet documented.
#define CPU_CACHE_IO            0xF0000000

/// Not yet documented.
#define CPU_CACHE_WAY_0_ADDR    0xC0000000
/// Not yet documented.
#define CPU_CACHE_WAY_1_ADDR    0xC0000400
/// Not yet documented.
#define CPU_CACHE_WAY_2_ADDR    0xC0000800
/// Not yet documented.
#define CPU_CACHE_WAY_3_ADDR    0xC0000C00

/// Not yet documented.
#define CPU_CACHE_2_WAY_SIZE    (CPU_CACHE_WAY_2_ADDR - CPU_CACHE_WAY_0_ADDR)

/// Not yet documented.
#define CPU_CACHE_MODE_4_WAY    0x00
/// Not yet documented.
#define CPU_CACHE_MODE_2_WAY    0x08

/// @}

/// @addtogroup CPU_CACHE_INLINE_FUNCTIONS
/// @{

/// @brief Not yet documented.
static inline void __always_inline
cpu_cache_enable(void)
{
        MEMORY_WRITE_OR(8, CPU(CCR), 0x01);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_cache_disable(void)
{
        MEMORY_WRITE_AND(8, CPU(CCR), ~0x01);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_cache_data_repl_disable(void)
{
        volatile uint8_t *reg_ccr;
        reg_ccr = (uint8_t *)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr & ~0x01;

        *reg_ccr = t0 | 0x04;
        *reg_ccr = t0 | 0x05;
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_cache_data_repl_enable(void)
{
        volatile uint8_t *reg_ccr;
        reg_ccr = (uint8_t *)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr & ~0x05;

        *reg_ccr = t0;
        *reg_ccr = t0 | 0x01;
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_cache_instr_repl_disable(void)
{
        volatile uint8_t *reg_ccr;
        reg_ccr = (uint8_t *)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr & ~0x01;

        *reg_ccr = t0 | 0x02;
        *reg_ccr = t0 | 0x03;
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_cache_instr_repl_enable(void)
{
        volatile uint8_t *reg_ccr;
        reg_ccr = (uint8_t *)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr & ~0x03;

        *reg_ccr = t0;
        *reg_ccr = t0 | 0x01;
}

/// @brief Not yet documented.
static inline void __always_inline
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

/// @}

/// @addtogroup CPU_CACHE_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void cpu_cache_purge_line(void *) __section(".uncached");
/// @brief Not yet documented.
extern void cpu_cache_purge(void) __section(".uncached");

/// @}

__END_DECLS

#endif /* !_CPU_CACHE_H */
