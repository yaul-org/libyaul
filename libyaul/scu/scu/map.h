/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCU_MAP_H_
#define _YAUL_SCU_MAP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/// @addtogroup MEMORY_MAP_MACROS
/// @{

/// Read value of size @p t from address @p x.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
#define MEMORY_READ(t, x)                                                      \
    (*(volatile uint ## t ## _t *)(x))

/// Write value @p y to address @p x of size @p t.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
/// @param y The value to write.
#define MEMORY_WRITE(t, x, y)                                                  \
do {                                                                           \
        (*(volatile uint ## t ## _t *)(x) = (y));                              \
} while (false)

/// Bitwise AND value @p y to address @p x of size @p t.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
/// @param y The value to bitwise AND.
#define MEMORY_WRITE_AND(t, x, y)                                              \
do {                                                                           \
        (*(volatile uint ## t ## _t *)(x) &= (y));                             \
} while (false)

/// Bitwise OR value @p y to address @p x of size @p t.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
/// @param y The value to bitwise OR.
#define MEMORY_WRITE_OR(t, x, y)                                               \
do {                                                                           \
        (*(volatile uint ## t ## _t *)(x) |= (y));                             \
} while (false)

/// @}

/// @addtogroup MEMORY_MAP_AREAS
/// @{

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define CS0(x)                  (0x22000000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define CS1(x)                  (0x24000000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define DUMMY(x)                (0x25000000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define CS2(x)                  (0x25800000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define CD_BLOCK(x)             (0x25890000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define SCSP(x)                 (0x25B00000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define VDP1_VRAM(x)            (0x25C00000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define VDP1_FB(x)              (0x25C80000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define VDP1(x)                 (0x25D00000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define VDP2_VRAM(x)            (0x25E00000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define VDP2_CRAM(x)            (0x25F00000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define VDP2(x)                 (0x25F80000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define SCU(x)                  (0x25FE0000UL + (x))

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define HWRAM(x)                (0x06000000UL + (x))

/// @brief Specify offset @p x for uncached address space.
///
/// @param x The byte offset.
#define HWRAM_UNCACHED(x)       (0x26000000UL + (x))

/// @brief Specify offset @p x to purge cache lines of address @p x.
///
/// @param x The byte offset.
#define HWRAM_PURGE_CACHE(x)    (0x46000000UL + (x))

/// Total size of H-WRAM in bytes
#define HWRAM_SIZE              0x00100000UL

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define LWRAM(x)                (0x00200000UL + (x))

/// @brief Specify offset @p x for uncached address space.
///
/// @param x The byte offset.
#define LWRAM_UNCACHED(x)       (0x20200000UL + (x))

/// @brief Specify offset @p x to purge cache lines of address @p x.
///
/// @param x The byte offset.
#define LWRAM_PURGE_CACHE(x)    (0x40200000UL + (x))

/// Total size of L-WRAM in bytes.
#define LWRAM_SIZE              0x00100000UL

/// Master CPU address for slave CPU notification
#define MINIT (0x21000000UL)
/// Save CPU address for master CPU notification
#define SINIT (0x21800000UL)

/// @}

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_SCU_IO_REGISTERS SCU I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D0R     0x0000UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D0W     0x0004UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D0C     0x0008UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D0AD    0x000CUL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D0EN    0x0010UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D0MD    0x0014UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D1R     0x0020UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D1W     0x0024UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D1C     0x0028UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D1AD    0x002CUL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D1EN    0x0030UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D1MD    0x0034UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D2R     0x0040UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D2W     0x0044UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D2C     0x0048UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D2AD    0x004CUL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D2EN    0x0050UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define D2MD    0x0054UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DSTP    0x0060UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DSTA    0x007CUL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PPAF    0x0080UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PPD     0x0084UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PDA     0x0088UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PDD     0x008CUL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define T0C     0x0090UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define T1S     0x0094UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define T1MD    0x0098UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define IMS     0x00A0UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define IST     0x00A4UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define AIACK   0x00A8UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ASR0    0x00B0UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ASR1    0x00B4UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define AREF    0x00B8UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RSEL    0x00C4UL

/// @brief SCU I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VER     0x00C8UL

/// @}

#endif /* !_YAUL_SCU_MAP_H_ */
