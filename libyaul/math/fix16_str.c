/*-
 * Copyright (c) Authors of libfixmath
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdbool.h>
#include <ctype.h>

#include "fix16.h"

static const uint32_t _scales[8] = {
        /* 5 decimals is enough for full fix16_t precision */
        1,
        10,
        100,
        1000,
        10000,
        100000,
        100000,
        100000
};

static char *
_itoa_loop(char *buf, uint32_t scale, uint32_t value, bool skip)
{
        while (scale) {
                unsigned digit = (value / scale);

                if (!skip || digit || scale == 1) {
                        skip = false;
                        *buf++ = '0' + digit;
                        value %= scale;
                }

                scale /= 10;
        }

        return buf;
}

void
fix16_to_str(fix16_t value, char *buf, int decimals)
{
        uint32_t uvalue = (value >= 0) ? value : -value;

        if (value < 0) {
                *buf++ = '-';
        }

        /* Separate the integer and decimal parts of the value */
        unsigned intpart = uvalue >> 16;
        uint32_t frac_part = uvalue & 0xFFFF;
        uint32_t scale = _scales[decimals & 7];

        frac_part = frac_part * scale;

        if (frac_part >= scale) {
                /* Handle carry from decimal part */
                intpart++;
                frac_part -= scale;
        }

        /* Format integer part */
        buf = _itoa_loop(buf, 10000, intpart, true);

        /* Format decimal part (if any) */
        if (scale != 1) {
                *buf++ = '.';
                buf = _itoa_loop(buf, scale / 10, frac_part, false);
        }

        *buf = '\0';
}
