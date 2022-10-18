/*-
 * Copyright (c) 2019 Stephane Dallongeville
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

#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>

#include <fix16.h>

#define P01 (10)
#define P02 (100)
#define P03 (1000)
#define P04 (1000 * 10)
#define P05 (1000 * 100)
#define P06 (1000 * 1000)
#define P07 (1000 * 1000 * 10)
#define P08 (1000 * 1000 * 100)
#define P09 (1000 * 1000 * 1000)
#define P10 (1000 * 1000 * 1000 * 10)

static const char _uppercase_hex_chars[] = "0123456789ABCDEF";
static const char _lowercase_hex_chars[] = "0123456789abcdef";

static int
_skip_atoi(const char * restrict *s)
{
        int i;

        for (i = 0; isdigit(**s); ) {
                i = ((i * 10) + *((*s)++) - '0');
        }

        return i;
}

static void
_out(FILE * restrict f, const void * restrict s, size_t l)
{
        f->write(f, (void *)s, l);
}

int
vfprintf(FILE * restrict f, const char * restrict fmt, va_list ap)
{
        char tmp_buffer[16];

        int32_t i;
        int32_t len;
        int32_t *ip;

        uint32_t num;
        num = 0;

        char *buf;
        buf = NULL;

        const char *hex_chars;

        int32_t left_align;
        int32_t plus_sign;
        int32_t zero_pad;
        int32_t space_sign;
        int32_t field_width;
        int32_t precision;

        size_t char_count;

        for (char_count = 0; *fmt; ++fmt) {
                if (*fmt != '%') {
                        _out(f, fmt, 1);
                        char_count++;

                        continue;
                }

                space_sign = zero_pad = plus_sign = left_align = 0;

                /* Process the flags */
        repeat:
                ++fmt; /* This also skips first '%' */

                switch (*fmt) {
                case '-':
                        left_align = 1;
                        goto repeat;

                case '+':
                        plus_sign = 1;
                        goto repeat;

                case ' ':
                        if (!plus_sign) {
                                space_sign = 1;
                        }

                        goto repeat;

                case '0':
                        zero_pad = 1;
                        goto repeat;
                }

                /* Process field width and precision */

                field_width = precision = -1;

                if (isdigit(*fmt)) {
                        field_width = _skip_atoi(&fmt);
                } else if (*fmt == '*') {
                        ++fmt;
                        /* It's the next argument */
                        field_width = va_arg(ap, int32_t);

                        if (field_width < 0) {
                                field_width = -field_width;
                                left_align = 1;
                        }
                }

                if (*fmt == '.') {
                        ++fmt;

                        if (isdigit(*fmt)) {
                                precision = _skip_atoi(&fmt);
                        } else if (*fmt == '*') {
                                ++fmt;
                                /* it's the next argument */
                                precision = va_arg(ap, int32_t);
                        }

                        if (precision < 0) {
                                precision = 0;
                        }
                }

                if ((*fmt == 'h') || (*fmt == 'l') || (*fmt == 'L')) {
                        ++fmt;
                }

                if (left_align != 0) {
                        zero_pad = 0;
                }

                switch (*fmt) {
                case 'c':
                        if (left_align == 0) {
                                while (--field_width > 0) {
                                        _out(f, " ", 1);
                                        char_count++;
                                }
                        }

                        const uint8_t value = (uint8_t)va_arg(ap, int32_t);

                        _out(f, &value, 1);
                        char_count++;

                        while (--field_width > 0) {
                                _out(f, " ", 1);
                                char_count++;
                        }

                        continue;
                case 's':
                        buf = va_arg(ap, char *);

                        if (buf == NULL) {
                                buf = "<NULL>";
                        }

                        len = strnlen(buf, precision);

                        if (left_align == 0)
                                while (len < field_width--) {
                                        _out(f, " ", 1);
                                        char_count++;
                                }

                        _out(f, buf, len);
                        char_count += len;

                        while (len < field_width--) {
                                _out(f, " ", 1);
                                char_count++;
                        }

                        continue;
                case 'p':
                        if (field_width == -1) {
                                field_width = 2 * sizeof(void *);
                                zero_pad = 1;
                        }

                        hex_chars = _uppercase_hex_chars;

                        goto hexa_conv;
                case 'x':
                        hex_chars = _lowercase_hex_chars;

                        goto hexa_conv;
                case 'X':
                        hex_chars = _uppercase_hex_chars;

                hexa_conv:
                        buf = &tmp_buffer[12];
                        *--buf = '\0';
                        num = va_arg(ap, uint32_t);

                        if (num == 0) {
                                *--buf = '0';
                        }

                        while (num != 0) {
                                *--buf = hex_chars[num & 0xF];
                                num >>= 4;
                        }

                        num = plus_sign = 0;

                        break;
                case 'n':
                        ip = va_arg(ap, int32_t *);
                        *ip = char_count;

                        continue;
                case 'u':
                        buf = &tmp_buffer[12];
                        *--buf = 0;
                        num = va_arg(ap, uint32_t);

                        if (num == 0) {
                                *--buf = '0';
                        }

                        while (num != 0) {
                                *--buf = (num % 10) + 0x30;
                                num /= 10;
                        }

                        num = plus_sign = 0;

                        break;
                case 'd':
                case 'i':
                        buf = &tmp_buffer[12];
                        *--buf = '\0';
                        i = va_arg(ap, int32_t);

                        if (i == 0) {
                                *--buf = '0';
                        }

                        if (i < 0) {
                                num = 1;

                                while (i != 0) {
                                        *--buf = 0x30 - (i % 10);
                                        i /= 10;
                                }
                        } else {
                                num = 0;

                                while (i != 0) {
                                        *--buf = (i % 10) + 0x30;
                                        i /= 10;
                                }
                        }

                        break;
                case 'f':
                        i = va_arg(ap, int32_t);

                        fix16_str((fix16_t)i, tmp_buffer, 7);
                        buf = tmp_buffer;

                        break;
                default:
                        continue;
                }

                len = strnlen(buf, precision);

                if (num != 0) {
                        _out(f, "-", 1);
                        char_count++;
                        field_width--;
                } else if (plus_sign != 0) {
                        _out(f, "+", 1);
                        char_count++;
                        field_width--;
                } else if (space_sign != 0) {
                        _out(f, " ", 1);
                        char_count++;
                        field_width--;
                }

                if (left_align == 0) {
                        if (zero_pad != 0) {
                                while (len < field_width--) {
                                        _out(f, "0", 1);
                                        char_count++;
                                }
                        } else {
                                while (len < field_width--) {
                                        _out(f, " ", 1);
                                        char_count++;
                                }
                        }
                }

                _out(f, buf, len);
                char_count += len;

                while (len < field_width--) {
                        _out(f, " ", 1);
                        char_count++;
                }
        }

        _out(f, "", 1);

        return char_count;
}
