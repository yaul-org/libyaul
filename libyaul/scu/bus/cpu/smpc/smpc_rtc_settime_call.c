/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <smpc/rtc.h>

#include "smpc_internal.h"

/* Command type: RTC. */
uint8_t
smpc_rtc_settime_call(struct smpc_time *time)
{

        time->month = (time->month) ? time->month : 0x1;

        MEM_POKE(IREG(0), time->year & 0x7F);
        MEM_POKE(IREG(1), (time->year >> 8) & 0x7F);
        MEM_POKE(IREG(2), time->day << 4);
        MEM_POKE(IREG(2), time->month);
        MEM_POKE(IREG(3), time->days);
        MEM_POKE(IREG(4), time->hour);
        MEM_POKE(IREG(5), time->minute);
        MEM_POKE(IREG(6), time->seconds);

        smpc_cmd_call(SMPC_RTC_SETTIME, SMPC_CMD_ISSUE_TYPE_C, NULL);

        return MEM_READ(OREG(31));
}
