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

#ifndef _YAUL_FLASH_H_
#define _YAUL_FLASH_H_

#include <scu/map.h>
#include <flash/map.h>

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

/// @defgroup FLASH Flashing related

/// @addtogroup FLASH
/// @{

/// @brief Not yet documented.
#define FLASH_CMD_ID_ENTRY  0x9090

/// @brief Not yet documented.
#define FLASH_CMD_ID_EXIT   0xF0F0

/// @brief Not yet documented.
#define FLASH_CMD_PAGEWRITE 0xA0A0

/// @brief Not yet documented.
static inline void __always_inline
flash_unlock(void)
{
        MEMORY_WRITE(16, FLASH(ADDR_55), 0xAAAA);
        MEMORY_WRITE(16, FLASH(ADDR_AA), 0x5555);
}

/// @brief Not yet documented.
static inline void __always_inline
flash_command_write(const uint16_t command)
{
        flash_unlock();

        MEMORY_WRITE(16, FLASH(ADDR_55), command);
}

/// @brief Not yet documented.
static inline uint16_t __always_inline
flash_vendor_get(void)
{
        flash_command_write(FLASH_CMD_ID_ENTRY);
        const uint16_t vendor_id = MEMORY_READ(16, FLASH(VENDOR_ID));
        flash_command_write(FLASH_CMD_ID_EXIT);

        return vendor_id;
}

/// @brief Not yet documented.
static inline uint16_t __always_inline
flash_device_get(void)
{
        flash_command_write(FLASH_CMD_ID_ENTRY);
        const uint16_t device_id = MEMORY_READ(16, FLASH(VENDOR_ID));
        flash_command_write(FLASH_CMD_ID_EXIT);

        return device_id;
}

/// @}

__END_DECLS

#endif /* !_YAUL_FLASH_H_ */
