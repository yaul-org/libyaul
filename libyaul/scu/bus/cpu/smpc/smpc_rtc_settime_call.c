/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/rtc.h>
#include <smpc/smc.h>

#include "smpc-internal.h"

/* Command type: RTC */
uint8_t
smpc_rtc_settime_call(struct smpc_time *time)
{

        time->month = (time->month) ? time->month : 0x1;

        MEMORY_WRITE(8, IREG(0), time->year & 0x7F);
        MEMORY_WRITE(8, IREG(1), (time->year >> 8) & 0x7F);
        MEMORY_WRITE(8, IREG(2), time->day << 4);
        MEMORY_WRITE(8, IREG(2), time->month);
        MEMORY_WRITE(8, IREG(3), time->days);
        MEMORY_WRITE(8, IREG(4), time->hours);
        MEMORY_WRITE(8, IREG(5), time->minutes);
        MEMORY_WRITE(8, IREG(6), time->seconds);

        return smpc_smc_call(SMPC_RTC_SETTIME);
}
