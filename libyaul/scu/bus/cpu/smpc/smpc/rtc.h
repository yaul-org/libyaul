/*
 * Copyright (c) Israel Jacquez
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
typedef enum smpc_time_month {
        SMPC_TIME_JANUARY = 1,
        SMPC_TIME_FEBRUARY,
        SMPC_TIME_MARCH,
        SMPC_TIME_APRIL,
        SMPC_TIME_MAY,
        SMPC_TIME_JUNE,
        SMPC_TIME_JULY,
        SMPC_TIME_AUGUST,
        SMPC_TIME_SEPTEMBER,
        SMPC_TIME_OCTOBER,
        SMPC_TIME_NOVEMBER,
        SMPC_TIME_DECEMBER
} smpc_time_month_t;

/// @brief Not yet documented.
typedef enum smpc_time_week_day {
        SMPC_TIME_SUNDAY,
        SMPC_TIME_MONDAY,
        SMPC_TIME_TUESDAY,
        SMPC_TIME_WEDNESDAY,
        SMPC_TIME_THURSDAY,
        SMPC_TIME_FRIDAY,
        SMPC_TIME_SATURDAY
} smpc_time_week_day_t;

/// @brief Not yet documented.
typedef struct smpc_time {
        /// Not yet documented.
        uint16_t year;
        /// Not yet documented.
        smpc_time_week_day_t week_day;
        /// Not yet documented.
        smpc_time_month_t month;
        /// Not yet documented.
        uint8_t day;
        /// Not yet documented.
        uint8_t hours;
        /// Not yet documented.
        uint8_t minutes;
        /// Not yet documented.
        uint8_t seconds;
} smpc_time_t;

/// @brief Not yet documented.
typedef struct smpc_time_dec {
        /// Not yet documented.
        uint16_t year;
        /// Not yet documented.
        smpc_time_week_day_t week_day;
        /// Not yet documented.
        smpc_time_month_t month;
        /// Not yet documented.
        uint8_t day;
        /// Not yet documented.
        uint8_t hours;
        /// Not yet documented.
        uint8_t minutes;
        /// Not yet documented.
        uint8_t seconds;
} smpc_time_dec_t;

/// @brief Not yet documented.
extern uint8_t smpc_rtc_settime_call(const smpc_time_t *time);

/// @brief Not yet documented.
extern const smpc_time_t *smpc_rtc_time_get(void);

/// @brief Not yet documented.
extern void smpc_rtc_time_bcd_from(const smpc_time_t *time,
    smpc_time_dec_t *time_dec);

/// @brief Not yet documented.
extern void smpc_rtc_time_bcd_to(const smpc_time_dec_t *time_dec,
    smpc_time_t *time);

/// @}

__END_DECLS

#endif /* !_YAUL_SMPC_RTC_H_ */
