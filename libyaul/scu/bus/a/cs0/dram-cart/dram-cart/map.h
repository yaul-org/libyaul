/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_DRAM_CART_MAP_H_
#define _YAUL_DRAM_CART_MAP_H_

#include <scu/map.h>

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_DRAM_CART_IO_REGISTERS DRAM Cart I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief DRAM Cart I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ID      0x00FFFFFFUL

/// @brief DRAM Cart I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define UNKNOWN 0x007EFFFEUL

/// @}

/// @addtogroup MEMORY_MAP_AREAS
/// @{

/// @brief Specify triple for DRAM address space.
///
/// @param t The two DRAM banks when 32-Mbit.
/// @param b The four DRAM banks within two larger DRAM banks, 8-MBit each.
/// @param x The byte offset.
#define DRAM(t, b, x)                                                          \
    (CS0(x) | ((((t) & 0x01) + 2) << 21) | (((b) & 0x03) << 19))

/// @}

#endif /* _YAUL_DRAM_CART_MAP_H_ */
