/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SMPC_RTC_H_
#define _YAUL_SMPC_RTC_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @defgroup SMPC_RTC SMPC RTC
/// Not yet documented.

/// @addtogroup SMPC_RTC
/// @{

/// @brief Not yet documented.
typedef struct cpu_smpc_time {
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
} cpu_smpc_time_t;

/// @}

__END_DECLS

#endif /* !_YAUL_SMPC_RTC_H_ */
