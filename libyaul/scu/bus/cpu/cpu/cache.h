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

/// @defgroup CPU_CACHE CPU Cache

/// @addtogroup CPU_CACHE
/// @{

/// Partition designated for using the cache.
#define CPU_CACHE               0x00000000UL
/// Partition designated for bypassing the cache. 
#define CPU_CACHE_THROUGH       0x20000000UL
/// Partition designated for purging a specific line.
#define CPU_CACHE_PURGE         0x40000000UL
/// Partition designated for accessing the address cache array directly.
#define CPU_CACHE_ADDRESS_RW    0x60000000UL
/// Partition designated for accessing the cache data directly.
#define CPU_CACHE_DATA_RW       0xC0000000UL
/// Partition designated for bypassing the cache (I/O area).
#define CPU_CACHE_IO            0xF0000000UL

/// Address for when accessing cache data directly.
#define CPU_CACHE_WAY_0_ADDR    0xC0000000UL
/// Address for when accessing cache data directly.
#define CPU_CACHE_WAY_1_ADDR    0xC0000400UL
/// Address for when accessing cache data directly.
#define CPU_CACHE_WAY_2_ADDR    0xC0000800UL
/// Address for when accessing cache data directly.
#define CPU_CACHE_WAY_3_ADDR    0xC0000C00UL

/// @brief The size in bytes of the 2KiB RAM.
/// @see cpu_cache_way_mode_set
#define CPU_CACHE_2_WAY_SIZE    (CPU_CACHE_WAY_2_ADDR - CPU_CACHE_WAY_0_ADDR)

/// @deprecated To be removed and replaced as an `enum`.
#define CPU_CACHE_MODE_4_WAY    0x00
/// @deprecated To be removed and replaced as an `enum`.
#define CPU_CACHE_MODE_2_WAY    0x08

/// @brief Enable cache.
static inline void __always_inline
cpu_cache_enable(void)
{
        MEMORY_WRITE_OR(8, CPU(CCR), 0x01);
}

/// @brief Disable cache.
static inline void __always_inline
cpu_cache_disable(void)
{
        MEMORY_WRITE_AND(8, CPU(CCR), ~0x01);
}

/// @brief Disable data replacement in the cache.
///
/// @details When data is fetched from memory, cache data is not
/// written to the cache even if there is a cache miss.
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

/// @brief Enable data replacement in the cache.
///
/// @details When data is fetched from memory, cache data is written.
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

/// @brief Disable instruction replacement in the cache.
///
/// @details When an instruction is fetched from memory, cache data is not
/// written to the cache even if there is a cache miss.
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

/// @brief Enable instruction replacement in the cache.
///
/// @details When an instruction is fetched from memory, cache data is written.
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

/// @brief Change the mode the cache operates.
///
/// @details The cache can be set to operate as a four-way set associative
/// cache, or as a two-way associative cache and 2KiB RAM.
///
/// In the two-way mode, ways `0` and `1` are RAM.
///
/// @param mode Way mode.
///
/// @see CPU_CACHE_MODE_4_WAY
/// @see CPU_CACHE_MODE_2_WAY
static inline void __always_inline
cpu_cache_way_mode_set(uint8_t mode)
{
        volatile uint8_t *reg_ccr;
        reg_ccr = (uint8_t *)CPU(CCR);

        uint8_t t0;
        t0 = *reg_ccr & ~0x0F;

        *reg_ccr = t0;
        *reg_ccr = t0 | mode;
        *reg_ccr = t0 | mode | 0x01;
}

/// @brief Cache line of the specified address is purged.
///
/// @details Calling this function will not pollute the cache.
/// 
/// @param addr Address associated with cache line.
extern void cpu_cache_purge_line(void *addr) __section(".uncached");

/// @brief Purge the entire cache.
///
/// @details All cache entries and all valid bits and LRU bits of all ways are
/// initialized to `0`. Calling this function will not pollute the cache.
extern void cpu_cache_purge(void) __no_reorder __section(".uncached");

/// @}

__END_DECLS

#endif /* !_CPU_CACHE_H */
