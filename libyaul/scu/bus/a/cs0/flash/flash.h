/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

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

/// @brief Test.
#define FLASH_CMD_ID_ENTRY  0x9090

/// @brief Test.
#define FLASH_CMD_ID_EXIT   0xF0F0

/// @brief Test.
#define FLASH_CMD_PAGEWRITE 0xA0A0

/// @brief Test.
static inline void __always_inline
flash_unlock(void)
{
        MEMORY_WRITE(16, FLASH(_55), 0xAAAA);
        MEMORY_WRITE(16, FLASH(_AA), 0x5555);
}

/// @brief Test.
static inline void __always_inline
flash_command_write(const uint16_t command)
{
        flash_unlock();

        MEMORY_WRITE(16, FLASH(_55), command);
}

/// @brief Test.
static inline uint16_t __always_inline
flash_vendor_get(void)
{
        flash_command_write(FLASH_CMD_ID_ENTRY);
        const uint16_t vendor_id = MEMORY_READ(16, FLASH(VENDOR_ID));
        flash_command_write(FLASH_CMD_ID_EXIT);

        return vendor_id;
}

/// @brief Test.
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
