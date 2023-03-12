/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCU_MAP_H_
#define _YAUL_SCU_MAP_H_

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/// @addtogroup MEMORY_MAP_MACROS
/// @{

/// Read value of size @p t from address @p x.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
#define MEMORY_READ(t, x) (*(volatile uint ## t ## _t *)(x))

/// Write value @p y to address @p x of size @p t.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
/// @param y The value to write.
#define MEMORY_WRITE(t, x, y) do {                                             \
    (*(volatile uint ## t ## _t *)(x) = (y));                                  \
} while (false)

/// Bitwise AND value @p y to address @p x of size @p t.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
/// @param y The value to bitwise AND.
#define MEMORY_WRITE_AND(t, x, y) do {                                         \
    (*(volatile uint ## t ## _t *)(x) &= (y));                                 \
} while (false)

/// Bitwise OR value @p y to address @p x of size @p t.
///
/// @param t The bit count: 8, 16, or 32.
/// @param x The byte offset.
/// @param y The value to bitwise OR.
#define MEMORY_WRITE_OR(t, x, y) do {                                          \
    (*(volatile uint ## t ## _t *)(x) |= (y));                                 \
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

/// @brief Base VDP1 address, for use with @ref vdp1_ioregs_t.
#define VDP1_IOREG_BASE         VDP1(0x00000000)

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

/// @brief Base VDP2 address, for use with @ref vdp2_ioregs_t.
#define VDP2_IOREG_BASE         VDP2(0x00000000)

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define SCU(x)                  (0x25FE0000UL + (x))

/// @brief Base SCU address, for use with @ref scu_ioregs_t.
#define SCU_IOREG_BASE          SCU(0x00000000)

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

/// @brief SCU DMA I/O register map.
/// @see SCU_DMA_IOREG_BASE
typedef struct scu_dma_ioregs {
    /// @brief SCU I/O register.
    uint32_t dnr;
    /// @brief SCU I/O register.
    uint32_t dnw;
    /// @brief SCU I/O register.
    uint32_t dnc;
    /// @brief SCU I/O register.
    uint32_t dnad;
    /// @brief SCU I/O register.
    uint32_t dnen;
    /// @brief SCU I/O register.
    uint32_t dnmd;
    unsigned int :32;
    unsigned int :32;
} __aligned(4) __packed scu_dma_ioregs_t;

/// @brief SCU I/O register map.
/// @see SCU_IOREG_BASE
typedef union scu_ioregs {
    /// @brief SCU I/O register buffer.
    uint32_t buffer[52];

    struct {
        union {
            struct {
                /// @brief SCU DMA Level 0 I/O registers.
                scu_dma_ioregs_t level0;
                /// @brief SCU DMA Level 1 I/O registers.
                scu_dma_ioregs_t level1;
                /// @brief SCU DMA Level 2 I/O registers.
                scu_dma_ioregs_t level2;
            };

            /// @brief SCU DMA Levels I/O registers.
            scu_dma_ioregs_t levels[3];
        };

        /// @brief SCU I/O register.
        uint32_t dstp;  /* 0x0060 */
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        /// @brief SCU I/O register.
        uint32_t dsta;  /* 0x007C */
        /// @brief SCU I/O register.
        uint32_t ppaf;  /* 0x0080 */
        /// @brief SCU I/O register.
        uint32_t ppd;   /* 0x0084 */
        /// @brief SCU I/O register.
        uint32_t pda;   /* 0x0088 */
        /// @brief SCU I/O register.
        uint32_t pdd;   /* 0x008C */
        /// @brief SCU I/O register.
        uint32_t t0c;   /* 0x0090 */
        /// @brief SCU I/O register.
        uint32_t t1s;   /* 0x0094 */
        /// @brief SCU I/O register.
        uint32_t t1md;  /* 0x0098 */
        unsigned int :32;
        /// @brief SCU I/O register.
        uint32_t ims;   /* 0x00A0 */
        /// @brief SCU I/O register.
        uint32_t ist;   /* 0x00A4 */
        /// @brief SCU I/O register.
        uint32_t aiack; /* 0x00A8 */
        unsigned int :32;
        /// @brief SCU I/O register.
        uint32_t asr0;  /* 0x00B0 */
        /// @brief SCU I/O register.
        uint32_t asr1;  /* 0x00B4 */
        /// @brief SCU I/O register.
        uint32_t aref;  /* 0x00B8 */
        unsigned int :32;
        unsigned int :32;
        /// @brief SCU I/O register.
        uint32_t rsel;  /* 0x00C4 */
        /// @brief SCU I/O register.
        uint32_t ver;   /* 0x00C8 */
        unsigned int :32;
    };
} __aligned(4) __packed scu_ioregs_t;

/// @}

#endif /* !_YAUL_SCU_MAP_H_ */
