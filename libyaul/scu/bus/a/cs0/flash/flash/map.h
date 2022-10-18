/*-
 * Copyright (c) 2012, 2015 Anders Montonen
 *
 * Original software by ExCyber. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. */

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
#define ADDR_55   0x0000AAAAUL

/// @brief Flash sees this as 0x2AAA.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ADDR_AA   0x00005554UL

/// @}

#endif /* !_YAUL_FLASH_MAP_H_ */
