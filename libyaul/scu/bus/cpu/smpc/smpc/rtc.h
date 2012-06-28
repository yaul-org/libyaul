/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _RTC_H_
#define _RTC_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct smpc_time {
        uint16_t year;
        uint8_t day;
        uint8_t month;
        uint8_t days;
        uint8_t hour;
        uint8_t minute;
        uint8_t seconds;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_RTC_H_ */
