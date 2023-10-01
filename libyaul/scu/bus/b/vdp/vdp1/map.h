/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP1_MAP_H_
#define _YAUL_VDP1_MAP_H_

#include <sys/cdefs.h>

#include <stdint.h>

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_VDP1_IO_REGISTERS VDP1 I/O
/// @ingroup MEMORY_MAP
/// @{


/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TVMR 0x0000UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FBCR 0x0002UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PTMR 0x0004UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define EWDR 0x0006UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define EWLR 0x0008UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define EWRR 0x000AUL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ENDR 0x000CUL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define EDSR 0x0010UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LOPR 0x0012UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COPR 0x0014UL

/// @brief VDP1 I/O register.
/// @see VDP1
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MODR 0x0016UL

/// @brief VDP1 I/O register map.
/// @see VDP1_IOREG_BASE
typedef union vdp1_ioregs {
    /// @brief VDP1 I/O register buffer.
    uint16_t buffer[12];

    struct {
        /// @brief VDP1 I/O register.
        uint16_t tvmr;
        /// @brief VDP1 I/O register.
        uint16_t fbcr;
        /// @brief VDP1 I/O register.
        uint16_t ptmr;
        /// @brief VDP1 I/O register.
        uint16_t ewdr;
        /// @brief VDP1 I/O register.
        uint16_t ewlr;
        /// @brief VDP1 I/O register.
        uint16_t ewrr;
        /// @brief VDP1 I/O register.
        uint16_t endr;
        unsigned int :16;
        /// @brief VDP1 I/O register.
        uint16_t edsr;
        /// @brief VDP1 I/O register.
        uint16_t lopr;
        /// @brief VDP1 I/O register.
        uint16_t copr;
        /// @brief VDP1 I/O register.
        uint16_t modr;
    };
} __aligned(2) __packed vdp1_ioregs_t;

/// @}

/// @brief Not yet documented.
typedef uintptr_t vdp1_vram_t;

#endif /* !_YAUL_VDP1_MAP_H_ */
