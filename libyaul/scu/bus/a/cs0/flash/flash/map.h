/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_FLASH_MAP_H_
#define _YAUL_FLASH_MAP_H_

/// @addtogroup MEMORY_MAP_AREAS
/// @{

/// @brief Specify offset @p x for address space.
///
/// @param x The byte offset.
#define FLASH(x) (0x22000000UL + (x))

/// @}

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_FLASH_IO_REGISTERS FLASH I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief Flash ID vendor.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VENDOR_ID 0x00000000UL

/// @brief Flash ID device.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DEVICE_ID 0x00000000UL

/// @brief Flash sees this as 0x5555.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define _55       0x0000AAAAUL

/// @brief Flash sees this as 0x2AAA.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define _AA       0x00005554UL

/// @}

#endif /* !_YAUL_FLASH_MAP_H_ */
