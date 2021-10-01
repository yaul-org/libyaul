/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_DRAM_CART_H_
#define _YAUL_DRAM_CART_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

__BEGIN_DECLS

/// @defgroup DRAM_CART DRAM Cart
///
/// @details Below is a map layout of when 8-Mbit or 32-Mbit.
/// @code
///              8-Mbit DRAM            32-Mbit DRAM
///             +--------------------+ +--------------------+
/// 0x2240'0000 | DRAM #0            | | DRAM #0            |
///             +--------------------+ |                    |
/// 0x2248'0000 | DRAM #0 (mirrored) | |                    |
///             +--------------------+ |                    |
/// 0x2250'0000 | DRAM #0 (mirrored) | |                    |
///             +--------------------+ |                    |
/// 0x2258'0000 | DRAM #0 (mirrored) | |                    |
///             +--------------------+ +--------------------+
/// 0x2260'0000 | DRAM #1            | | DRAM #1            |
///             +--------------------+ |                    |
/// 0x2268'0000 | DRAM #1 (mirrored) | |                    |
///             +--------------------+ |                    |
/// 0x2270'0000 | DRAM #1 (mirrored) | |                    |
///             +--------------------+ |                    |
/// 0x2278'0000 | DRAM #1 (mirrored) | |                    |
/// 0x227F'FFFF +--------------------+ +--------------------+
/// @endcode

/// @addtogroup DRAM_CART
/// @{

/// @brief DRAM cart ID.
typedef enum dram_cart_id {
        /// Invalid DRAM cart ID.
        DRAM_CART_ID_INVALID = 0x00,
        /// DRAM cart ID for cart size of 1MiB (8-MBit).
        DRAM_CART_ID_1MIB    = 0x5A,
        /// DRAM cart ID for cart size of 4MiB (32-MBit).
        DRAM_CART_ID_4MIB    = 0x5C
} dram_cart_id_t;

/// @brief Return base pointer for DRAM cart.
/// @returns The base DRAM cart pointer.
extern void *dram_cart_area_get(void);

/// @brief Obtain detected DRAM cart ID.
/// @return The detected DRAM cart ID.
extern dram_cart_id_t dram_cart_id_get(void);

/// @brief Obtain detected DRAM cart size.
/// @return The detected DRAM cart size.
extern size_t dram_cart_size_get(void);

/// @}

__END_DECLS

#endif /* !_YAUL_DRAM_CART_H_ */
