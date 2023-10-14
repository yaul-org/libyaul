/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/rtc.h>
#include <smpc/smc.h>

#include "smpc-internal.h"

static void _10_divide(uint8_t value, uint8_t *q, uint8_t *r);

static uint8_t _bcd_from(uint8_t value);
static uint8_t _bcd_to(uint8_t value);
static void _bcd_year_convert(uint16_t year, uint8_t *digits);

/* Command type: RTC */
uint8_t
smpc_rtc_settime_call(const smpc_time_t *time)
{
        MEMORY_WRITE(8, IREG(0), time->year >> 8);
        MEMORY_WRITE(8, IREG(1), time->year & 0xFF);
        MEMORY_WRITE(8, IREG(2), (time->day << 4) | time->month);
        MEMORY_WRITE(8, IREG(3), time->week_day);
        MEMORY_WRITE(8, IREG(4), time->hours);
        MEMORY_WRITE(8, IREG(5), time->minutes);
        MEMORY_WRITE(8, IREG(6), time->seconds);

        return smpc_smc_call(SMPC_RTC_SETTIME);
}

const smpc_time_t *
smpc_rtc_time_get(void)
{
        return &__smpc_time;
}

void
smpc_rtc_time_bcd_from(const smpc_time_t *time, smpc_time_dec_t *time_dec)
{
        time_dec->year = (_bcd_from(time->year >> 8) * 100) + _bcd_from(time->year & 0xFF);
        time_dec->week_day = time->week_day;
        time_dec->month = time->month;
        time_dec->day = _bcd_from(time->day);
        time_dec->hours = _bcd_from(time->hours);
        time_dec->minutes = _bcd_from(time->minutes);
        time_dec->seconds = _bcd_from(time->seconds);
}

void
smpc_rtc_time_bcd_to(const smpc_time_dec_t *time_dec, smpc_time_t *time)
{
        uint8_t year_digits[4];

        _bcd_year_convert(time_dec->year, year_digits);

        time->year = (year_digits[0] << 12) |
                     (year_digits[1] << 8) |
                     (year_digits[2] << 4) |
                      year_digits[3];

        time->week_day = time_dec->week_day;
        time->month = time_dec->month;
        time->day = _bcd_to(time_dec->day);
        time->hours = _bcd_to(time_dec->hours);
        time->minutes = _bcd_to(time_dec->minutes);
        time->seconds = _bcd_to(time_dec->seconds);
}

uint8_t
_bcd_from(uint8_t value)
{
        if (value == 0x00) {
                return 0;
        }

        if (value >= 0x99) {
                return 99;
        }

        return (((value >> 4) * 10) + (value & 0x0F));
}

static uint8_t
_bcd_to(uint8_t value)
{
        /* We're limiting the range from 0..99 */
        if (value == 0) {
                return 0x00;
        }

        if (value > 99) {
                return 0x99;
        }

        uint8_t ones;
        uint8_t tens;

        _10_divide(value, &tens, &ones);

        return ((tens << 4) | ones);
}

/* Algorithm taken from: <http://homepage.cs.uiowa.edu/~dwjones/bcd/decimal.html> */
static void
_bcd_year_convert(uint16_t year, uint8_t *digits)
{
        uint8_t * const d0 = &digits[0];
        uint8_t * const d1 = &digits[1];
        uint8_t * const d2 = &digits[2];
        uint8_t * const d3 = &digits[3];

        *d0 = year         & 0x1F;
        *d1 = (year >> 5)  & 0x0F;
        *d2 = (year >> 9)  & 0x0F;
        *d3 = (year >> 13) & 0x07;

        uint8_t q;
        *d0 = 2 * (*d3 + *d2 + *d1) + *d0;
        q = (*d0 * (uint16_t)0x67) >> 10;
        *d0 = *d0 - 10 * q;

        *d1 = 9 * *d3 + *d2 + 3 * *d1 + q;
        q = (*d1 * (uint16_t)0x67) >> 10;
        *d1 = *d1 - 10 * q;

        *d2 = *d3 + 5 * *d2 + q;
        q = (*d2 * (uint16_t)0x67) >> 10;
        *d2 = *d2 - 10 * q;

        *d3 = 8 * *d3 + q;
        q = (*d3 * (uint16_t)0x1A) >> 8;
        *d3 = *d3 - 10 * q;
}

/* Algorithm taken from: <http://homepage.cs.uiowa.edu/~dwjones/bcd/decimal.html> */
static void
_10_divide(uint8_t value, uint8_t *q, uint8_t *r)
{
        /* Approximate the quotient as q = value * 0.00011001101 (binary) */
        *q = ((value >> 2) + value) >> 1; /* Times 0.101 */
        *q = ((*q        ) + value) >> 1; /* Times 0.1101 */
        *q = ((*q    >> 2) + value) >> 1; /* Times 0.1001101 */
        *q = ((*q        ) + value) >> 4; /* Times 0.00011001101 */

        /* Compute the reimainder as r = value - 10 * q */
        *r = ((*q << 2) + *q) << 1; /* Times 1010 */
        *r = value - *r;
}
