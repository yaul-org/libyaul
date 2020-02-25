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

/// @addtogroup SCU_MAP_HELPERS
/// @{

/// Read value of size @p t from address @x
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
#define CS0(x)                  (0x22000000 + (x))
/// Specify offset @p x for address space.
#define CS1(x)                  (0x24000000 + (x))
/// Specify offset @p x for address space.
#define DUMMY(x)                (0x25000000 + (x))
/// Specify offset @p x for address space.
#define CS2(x)                  (0x25800000 + (x))
/// Specify offset @p x for address space.
#define SCSP(x)                 (0x25B00000 + (x))
/// Specify offset @p x for address space.
#define VDP1_VRAM(x)            (0x25C00000 + (x))
/// Specify offset @p x for address space.
#define VDP1_FB(x)              (0x25C80000 + (x))
/// Specify offset @p x for address space.
#define VDP1(x)                 (0x25D00000 + (x))
/// Specify offset @p x for address space.
#define VDP2_VRAM(x)            (0x25E00000 + (x))
/// Specify offset @p x for address space.
#define VDP2_CRAM(x)            (0x25F00000 + (x))
/// Specify offset @p x for address space.
#define VDP2(x)                 (0x25F80000 + (x))
/// Specify offset @p x for address space.
#define SCU(x)                  (0x25FE0000 + (x))

/// Specify offset @p x for address space.
#define HWRAM(x)                (0x06000000 + (x))
/// Specify offset @p x for uncached address space.
#define HWRAM_UNCACHED(x)       (0x26000000 + (x))
/// Specify offset @p x to purge cache lines of address @p x.
#define HWRAM_PURGE_CACHE(x)    (0x46000000 + (x))
/// Total size of H-WRAM in bytes
#define HWRAM_SIZE              0x100000

/// Specify offset @p x for address space.
#define LWRAM(x)                (0x00200000 + (x))
/// Specify offset @p x for uncached address space.
#define LWRAM_UNCACHED(x)       (0x20200000 + (x))
/// Specify offset @p x to purge cache lines of address @p x.
#define LWRAM_PURGE_CACHE(x)    (0x40200000 + (x))
/// Total size of L-WRAM in bytes.
#define LWRAM_SIZE              0x100000

/// @}

#define D0R     0x0000
#define D0W     0x0004
#define D0C     0x0008
#define D0AD    0x000C
#define D0EN    0x0010
#define D0MD    0x0014
#define D1R     0x0020
#define D1W     0x0024
#define D1C     0x0028
#define D1AD    0x002C
#define D1EN    0x0030
#define D1MD    0x0034
#define D2R     0x0040
#define D2W     0x0044
#define D2C     0x0048
#define D2AD    0x004C
#define D2EN    0x0050
#define D2MD    0x0054
#define DSTP    0x0060
#define DSTA    0x007C
#define PPAF    0x0080
#define PPD     0x0084
#define PDA     0x0088
#define PDD     0x008C
#define T0C     0x0090
#define T1S     0x0094
#define T1MD    0x0098
#define IMS     0x00A0
#define IST     0x00A4
#define AIACK   0x00A8
#define ASR0    0x00B0
#define ASR1    0x00B4
#define AREF    0x00B8
#define RSEL    0x00C4
#define VER     0x00C8

#endif /* !_SCU_MAP_H_ */
