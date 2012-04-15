/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _RTC_H_
#define _RTC_H_

#include <inttypes.h>

struct smpc_time {
        uint16_t year;
        uint8_t day;
        uint8_t month;
        uint8_t days;
        uint8_t hour;
        uint8_t minute;
        uint8_t seconds;
};

#endif /* !_RTC_H_ */
