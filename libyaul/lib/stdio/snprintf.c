#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define P01 10
#define P02 100
#define P03 1000
#define P04 1000 * 10
#define P05 1000 * 100
#define P06 1000 * 1000
#define P07 1000 * 1000 * 10
#define P08 1000 * 1000 * 100
#define P09 1000 * 1000 * 1000
#define P10 1000 * 1000 * 1000 * 10

static const char uppercase_hexchars[] = "0123456789ABCDEF";
static const char lowercase_hexchars[] = "0123456789abcdef";
static const char digits[] __unused =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

static uint16_t digits10(const uint32_t v);
static uint16_t skip_atoi(const char **s);
static uint16_t vsprintf(char *buf, const char *fmt, va_list args);

static uint16_t __unused
digits10(const uint32_t v)
{
    if (v < P01) return 1;
    if (v < P02) return 2;
    if (v < P03) return 3;
    if (v < P08)
    {
        if (v < P06)
        {
            if (v < P04) return 4;
            return 5 + (v >= P05);
        }
        return 7 + (v >= P07);
    }
    return 9 + (v >= P09);
}

static uint16_t
skip_atoi(const char **s)
{
    uint16_t i = 0;

    while(isdigit(**s))
        i = (i * 10) + *((*s)++) - '0';

    return i;
}

static uint16_t
vsprintf(char *buf, const char *fmt, va_list args)
{
    char tmp_buffer[12];
    int32_t i;
    int16_t len;
    int16_t *ip;
    uint32_t num;
    char *s;
    const char *hexchars;
    char *str;
    int16_t left_align;
    int16_t plus_sign;
    int16_t zero_pad;
    int16_t space_sign;
    int16_t field_width;
    int16_t precision;

    for (str = buf; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        space_sign = zero_pad = plus_sign = left_align = 0;

        // Process the flags
repeat:
        ++fmt;          // this also skips first '%'

        switch (*fmt)
        {
        case '-':
            left_align = 1;
            goto repeat;

        case '+':
            plus_sign = 1;
            goto repeat;

        case ' ':
            if ( !plus_sign )
                space_sign = 1;

            goto repeat;

        case '0':
            zero_pad = 1;
            goto repeat;
        }

        // Process field width and precision

        field_width = precision = -1;

        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++fmt;
            // it's the next argument
            /* field_width = va_arg(args, int16_t); */
            field_width = va_arg(args, int);

            if (field_width < 0)
            {
                field_width = -field_width;
                left_align = 1;
            }
        }

        if (*fmt == '.')
        {
            ++fmt;

            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++fmt;
                // it's the next argument
                /* precision = va_arg(args, int16_t); */
                precision = va_arg(args, int);
            }

            if (precision < 0)
                precision = 0;
        }

        if ((*fmt == 'h') || (*fmt == 'l') || (*fmt == 'L'))
            ++fmt;

        if (left_align)
            zero_pad = 0;

        switch (*fmt)
        {
        case 'c':
            if (!left_align)
                while(--field_width > 0)
                    *str++ = ' ';

            /* *str++ = (unsigned char) va_arg(args, int16_t); */
            *str++ = (unsigned char) va_arg(args, int);

            while(--field_width > 0)
                *str++ = ' ';

            continue;

        case 's':
            s = va_arg(args, char *);

            if (!s)
                s = "<NULL>";

            len = strnlen(s, precision);

            if (!left_align)
                while(len < field_width--)
                    *str++ = ' ';

            for (i = 0; i < len; ++i)
                *str++ = *s++;

            while(len < field_width--)
                *str++ = ' ';

            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = 2 * sizeof(void *);
                zero_pad = 1;
            }

            hexchars = uppercase_hexchars;
            goto hexa_conv;

        case 'x':
            hexchars = lowercase_hexchars;
            goto hexa_conv;

        case 'X':
            hexchars = uppercase_hexchars;

hexa_conv:
            s = &tmp_buffer[12];
            *--s = 0;
            /* num = va_arg(args, uint16_t); */
            num = va_arg(args, int);

            if (!num)
                *--s = '0';

            while(num)
            {
                *--s = hexchars[num & 0xF];
                num >>= 4;
            }

            num = plus_sign = 0;

            break;

        case 'n':
            ip = va_arg(args, int16_t*);
            *ip = (str - buf);
            continue;

        case 'u':
            s = &tmp_buffer[12];
            *--s = 0;
            /* num = va_arg(args, uint16_t); */
            num = va_arg(args, int);

            if (!num)
                *--s = '0';

            while(num)
            {
                *--s = (num % 10) + 0x30;
                num /= 10;
            }

            num = plus_sign = 0;

            break;

        case 'd':
        case 'i':
            s = &tmp_buffer[12];
            *--s = 0;
            /* i = va_arg(args, int16_t); */
            i = va_arg(args, int32_t);

            if (!i)
                *--s = '0';

            if (i < 0)
            {
                num = 1;

                while(i)
                {
                    *--s = 0x30 - (i % 10);
                    i /= 10;
                }
            }
            else
            {
                num = 0;

                while(i)
                {
                    *--s = (i % 10) + 0x30;
                    i /= 10;
                }
            }

            break;

        default:
            continue;
        }

        len = strnlen(s, precision);

        if (num)
        {
            *str++ = '-';
            field_width--;
        }
        else if (plus_sign)
        {
            *str++ = '+';
            field_width--;
        }
        else if (space_sign)
        {
            *str++ = ' ';
            field_width--;
        }

        if ( !left_align)
        {
            if (zero_pad)
            {
                while(len < field_width--)
                    *str++ = '0';
            }
            else
            {
                while(len < field_width--)
                    *str++ = ' ';
            }
        }

        for (i = 0; i < len; ++i)
            *str++ = *s++;

        while(len < field_width--)
            *str++ = ' ';
    }

    *str = '\0';

    return str - buf;
}

int
sprintf(char * __restrict buffer, const char *__restrict fmt, ...)
{
    va_list args;
    uint16_t i;

    va_start(args, fmt);
    i = vsprintf(buffer, fmt, args);
    va_end(args);

    return i;
}
