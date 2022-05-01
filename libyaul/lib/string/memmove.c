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

#include <string.h>
#include <stdint.h>

#ifdef __GNUC__
typedef __may_alias size_t WT;
#define WS (sizeof(WT))
#endif /* __GNUC__ */

/* To avoid from LTO discarding memmove (as it's considered a builtin by GCC) */
void * __used
memmove(void *dest, const void *src, size_t n)
{
        char *d = dest;
        const char *s = src;

        if (d == s) {
                return d;
        }

        if ((uintptr_t)s - (uintptr_t)d - n <= -2 * n) {
                return memcpy(d, s, n);
        }

        if (d < s) {
#ifdef __GNUC__

                if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
                        while ((uintptr_t)d % WS) {
                                if (!n--) {
                                        return dest;
                                }

                                *d++ = *s++;
                        }

                        for (; n >= WS; n -= WS, d += WS, s += WS) {
                                *(WT *)d = *(WT *)s;
                        }
                }

#endif /* __GNUC__ */

                for (; n; n--) {
                        *d++ = *s++;
                }
        } else {
#ifdef __GNUC__
                if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
                        while ((uintptr_t)(d + n) % WS) {
                                if (!n--) {
                                        return dest;
                                }

                                d[n] = s[n];
                        }

                        while (n >= WS) {
                                n -= WS, *(WT *)(d + n) = *(WT *)(s + n);
                        }
                }

#endif /* __GNUC__ */

                while (n) {
                        n--, d[n] = s[n];
                }
        }

        return dest;
}
