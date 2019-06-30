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
        uint32_t fracpart = uvalue & 0xFFFF;
        uint32_t scale = _scales[decimals & 7];
        fracpart = fix16_mul(fracpart, scale);

        if (fracpart >= scale) {
                /* Handle carry from decimal part */
                intpart++;
                fracpart -= scale;
        }

        /* Format integer part */
        buf = _itoa_loop(buf, 10000, intpart, true);

        /* Format decimal part (if any) */
        if (scale != 1) {
                *buf++ = '.';
                buf = _itoa_loop(buf, scale / 10, fracpart, false);
        }

        *buf = '\0';
}
