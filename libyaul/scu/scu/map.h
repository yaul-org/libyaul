/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_MAP_H_
#define _SCU_MAP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/// @defgroup SCU_MAP_HELPERS
/// @defgroup SCU_MAP_OFFSETS
/// @defgroup SCU_MAP_REGISTERS

/// @addtogroup SCU_MAP_HELPERS
/// @{

/// Read value of size @p t from address @p x.
#define MEMORY_READ(t, x)               (*(volatile uint ## t ## _t *)(x))

/// Write value @p y to address @p x of size @p t.
#define MEMORY_WRITE(t, x, y) do {                                             \
        (*(volatile uint ## t ## _t *)(x) = (y));                              \
} while (false)

/// Bitwise AND value @p y to address @p x of size @p t.
#define MEMORY_WRITE_AND(t, x, y) do {                                         \
        (*(volatile uint ## t ## _t *)(x) &= (y));                             \
} while (false)

/// Bitwise OR value @p y to address @p x of size @p t.
#define MEMORY_WRITE_OR(t, x, y) do {                                          \
        (*(volatile uint ## t ## _t *)(x) |= (y));                             \
} while (false)

/// @}

/// @addtogroup SCU_MAP_OFFSETS
/// @{

/// Specify offset @p x for address space.
#define CS0(x)                  (0x22000000UL + (x))
/// Specify offset @p x for address space.
#define CS1(x)                  (0x24000000UL + (x))
/// Specify offset @p x for address space.
#define DUMMY(x)                (0x25000000UL + (x))
/// Specify offset @p x for address space.
#define CS2(x)                  (0x25800000UL + (x))
/// Specify offset @p x for address space.
#define SCSP(x)                 (0x25B00000UL + (x))
/// Specify offset @p x for address space.
#define VDP1_VRAM(x)            (0x25C00000UL + (x))
/// Specify offset @p x for address space.
#define VDP1_FB(x)              (0x25C80000UL + (x))
/// Specify offset @p x for address space.
#define VDP1(x)                 (0x25D00000UL + (x))
/// Specify offset @p x for address space.
#define VDP2_VRAM(x)            (0x25E00000UL + (x))
/// Specify offset @p x for address space.
#define VDP2_CRAM(x)            (0x25F00000UL + (x))
/// Specify offset @p x for address space.
#define VDP2(x)                 (0x25F80000UL + (x))
/// Specify offset @p x for address space.
#define SCU(x)                  (0x25FE0000UL + (x))

/// Specify offset @p x for address space.
#define HWRAM(x)                (0x06000000UL + (x))
/// Specify offset @p x for uncached address space.
#define HWRAM_UNCACHED(x)       (0x26000000UL + (x))
/// Specify offset @p x to purge cache lines of address @p x.
#define HWRAM_PURGE_CACHE(x)    (0x46000000UL + (x))
/// Total size of H-WRAM in bytes
#define HWRAM_SIZE              0x00100000UL

/// Specify offset @p x for address space.
#define LWRAM(x)                (0x00200000UL + (x))
/// Specify offset @p x for uncached address space.
#define LWRAM_UNCACHED(x)       (0x20200000UL + (x))
/// Specify offset @p x to purge cache lines of address @p x.
#define LWRAM_PURGE_CACHE(x)    (0x40200000UL + (x))
/// Total size of L-WRAM in bytes.
#define LWRAM_SIZE              0x00100000UL

/// @}

/// @addtogroup SCU_MAP_REGISTERS
/// @{

#define D0R     0x0000UL
#define D0W     0x0004UL
#define D0C     0x0008UL
#define D0AD    0x000CUL
#define D0EN    0x0010UL
#define D0MD    0x0014UL
#define D1R     0x0020UL
#define D1W     0x0024UL
#define D1C     0x0028UL
#define D1AD    0x002CUL
#define D1EN    0x0030UL
#define D1MD    0x0034UL
#define D2R     0x0040UL
#define D2W     0x0044UL
#define D2C     0x0048UL
#define D2AD    0x004CUL
#define D2EN    0x0050UL
#define D2MD    0x0054UL
#define DSTP    0x0060UL
#define DSTA    0x007CUL
#define PPAF    0x0080UL
#define PPD     0x0084UL
#define PDA     0x0088UL
#define PDD     0x008CUL
#define T0C     0x0090UL
#define T1S     0x0094UL
#define T1MD    0x0098UL
#define IMS     0x00A0UL
#define IST     0x00A4UL
#define AIACK   0x00A8UL
#define ASR0    0x00B0UL
#define ASR1    0x00B4UL
#define AREF    0x00B8UL
#define RSEL    0x00C4UL
#define VER     0x00C8UL

/// @}

#endif /* !_SCU_MAP_H_ */
