/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_ARP_MAP_H_
#define _YAUL_ARP_MAP_H_

/// @addtogroup MEMORY_MAP_AREAS
/// @{

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define ARP(x) (0x22000000UL + (x))

/// @}

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_ARP_IO_REGISTERS ARP I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief ARP I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define OUTPUT  0x00080001UL

/// @brief ARP I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define STATUS  0x00100001UL

/// @brief ARP I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define INPUT   0x00180001UL

/// @brief ARP I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VERSION 0x00004AF0UL

/// @}

#endif /* !_YAUL_ARP_MAP_H_ */
