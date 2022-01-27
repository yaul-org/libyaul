/*-
 * Copyright (c) 2005-2014 Rich Felker, et al.
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

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>

struct cookie {
        char *s;
        size_t n;
};

static size_t
_write(FILE *f, const unsigned char *s, size_t l)
{
        struct cookie * const cookie = f->cookie;

        for (size_t i = 0; i < l; i++) {
                *cookie->s++ = *s++;
        }

        return l;
}

int
vsnprintf(char * restrict s, size_t n, const char *fmt, va_list ap)
{
        char dummy[1];

        struct cookie cookie = {
                .s = (n != 0) ? s : dummy,
                .n = (n != 0) ? (n - 1) : 0
        };

        FILE f = {
                .write = _write,
                .cookie = &cookie
        };

        if (n > INT_MAX) {
                errno = EOVERFLOW;

                return -1;
        }

        *cookie.s = '\0';

        return vfprintf(&f, fmt, ap);
}
