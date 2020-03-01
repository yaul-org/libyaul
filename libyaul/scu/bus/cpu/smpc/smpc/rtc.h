/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SMPC_RTC_H_
#define _SMPC_RTC_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @defgroup SCU_DMA_STRUCTURES

/// @addtogroup SCU_DMA_STRUCTURES
/// @{

/// @struct
/// @brief Not yet documented.
struct smpc_time {
        /// Not yet documented.
        uint16_t year;
        /// Not yet documented.
        uint8_t day;
        /// Not yet documented.
        uint8_t month;
        /// Not yet documented.
        uint8_t days;
        /// Not yet documented.
        uint8_t hours;
        /// Not yet documented.
        uint8_t minutes;
        /// Not yet documented.
        uint8_t seconds;
};

/// @}

__END_DECLS

#endif /* !_SMPC_RTC_H_ */
