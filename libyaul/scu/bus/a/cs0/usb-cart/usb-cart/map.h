/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_USB_CART_MAP_H_
#define _YAUL_USB_CART_MAP_H_

#include <scu/map.h>

/// @addtogroup MEMORY_MAP_AREAS
/// @{

/// @brief Access the CPU I/O registers.
///
/// @param x The byte offset.
#define USB_CART(x) (CS0(0x01) + ((x) << 20))

/// @}

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_USB_CART_IO_REGISTERS USB Cart I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief USB Cart I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FIFO    0x01

/// @brief USB Cart I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FLAGS   0x02

/// @}

#endif /* !_YAUL_USB_CART_MAP_H_ */
