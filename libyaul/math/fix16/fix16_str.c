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

static char *_itoa_loop(char *buffer, uint32_t scale, uint32_t value, bool skip);

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

size_t
fix16_str(fix16_t value, char *buffer, int32_t decimals)
{
        const uint32_t uvalue = (value >= 0) ? value : -value;

        const char * const start_buffer = buffer;

        if (value < 0) {
                *buffer++ = '-';
        }

        /* Separate the integer and decimal parts of the value */
        uint32_t int_part;
        int_part = uvalue >> 16;

        uint32_t frac_part;
        frac_part = uvalue & 0xFFFF;

        const uint32_t scale = _scales[decimals & 7];

        frac_part = fix16_mul(frac_part, scale);

        if (frac_part >= scale) {
                /* Handle carry from decimal part */
                int_part++;
                frac_part -= scale;
        }

        /* Format integer part */
        buffer = _itoa_loop(buffer, 10000, int_part, true);

        /* Format decimal part (if any) */
        if (scale != 1) {
                *buffer++ = '.';
                buffer = _itoa_loop(buffer, scale / 10, frac_part, false);
        }

        *buffer = '\0';

        return (buffer - start_buffer);
}

static char *
_itoa_loop(char *buf, uint32_t scale, uint32_t value, bool skip)
{
        while (scale) {
                unsigned digit = (value / scale);

                if (!skip || digit || (scale == 1)) {
                        skip = false;

                        *buf++ = '0' + digit;

                        value %= scale;
                }

                scale /= 10;
        }

        return buf;
}
