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

/* To avoid from LTO discarding memcpy (as it's considered a builtin by GCC) */
void * __used
memcpy(void *restrict dest, const void *restrict src, size_t n)
{
        uint8_t *d = dest;
        const uint8_t *s = src;

#ifdef __GNUC__
        typedef uint32_t __may_alias a_uint32_t;

        uint32_t w, x;

        for (; (uintptr_t)s % 4 && n; n--) {
                *d++ = *s++;
        }

        if ((uintptr_t)d % 4 == 0) {
                for (; n >= 16; s += 16, d += 16, n -= 16) {
                        *(a_uint32_t *)(d + 0) = *(a_uint32_t *)(s + 0);
                        *(a_uint32_t *)(d + 4) = *(a_uint32_t *)(s + 4);
                        *(a_uint32_t *)(d + 8) = *(a_uint32_t *)(s + 8);
                        *(a_uint32_t *)(d + 12) = *(a_uint32_t *)(s + 12);
                }

                if (n & 8) {
                        *(a_uint32_t *)(d + 0) = *(a_uint32_t *)(s + 0);
                        *(a_uint32_t *)(d + 4) = *(a_uint32_t *)(s + 4);
                        d += 8;
                        s += 8;
                }

                if (n & 4) {
                        *(a_uint32_t *)(d + 0) = *(a_uint32_t *)(s + 0);
                        d += 4;
                        s += 4;
                }

                if (n & 2) {
                        *d++ = *s++;
                        *d++ = *s++;
                }

                if (n & 1) {
                        *d = *s;
                }

                return dest;
        }

        if (n >= 32) {
                switch ((uintptr_t)d % 4) {
                case 1:
                        w = *(a_uint32_t *)s;
                        *d++ = *s++;
                        *d++ = *s++;
                        *d++ = *s++;
                        n -= 3;

                        for (; n >= 17; s += 16, d += 16, n -= 16) {
                                x = *(a_uint32_t *)(s + 1);
                                *(a_uint32_t *)(d + 0) = (w << 24) | (x >> 8);
                                w = *(a_uint32_t *)(s + 5);
                                *(a_uint32_t *)(d + 4) = (x << 24) | (w >> 8);
                                x = *(a_uint32_t *)(s + 9);
                                *(a_uint32_t *)(d + 8) = (w << 24) | (x >> 8);
                                w = *(a_uint32_t *)(s + 13);
                                *(a_uint32_t *)(d + 12) = (x << 24) | (w >> 8);
                        }
                        break;
                case 2:
                        w = *(a_uint32_t *)s;
                        *d++ = *s++;
                        *d++ = *s++;
                        n -= 2;

                        for (; n >= 18; s += 16, d += 16, n -= 16) {
                                x = *(a_uint32_t *)(s + 2);
                                *(a_uint32_t *)(d + 0) = (w << 16) | (x >> 16);
                                w = *(a_uint32_t *)(s + 6);
                                *(a_uint32_t *)(d + 4) = (x << 16) | (w >> 16);
                                x = *(a_uint32_t *)(s + 10);
                                *(a_uint32_t *)(d + 8) = (w << 16) | (x >> 16);
                                w = *(a_uint32_t *)(s + 14);
                                *(a_uint32_t *)(d + 12) = (x << 16) | (w >> 16);
                        }
                        break;
                case 3:
                        w = *(a_uint32_t *)s;
                        *d++ = *s++;
                        n -= 1;

                        for (; n >= 19; s += 16, d += 16, n -= 16) {
                                x = *(a_uint32_t *)(s + 3);
                                *(a_uint32_t *)(d + 0) = (w << 8) | (x >> 24);
                                w = *(a_uint32_t *)(s + 7);
                                *(a_uint32_t *)(d + 4) = (x << 8) | (w >> 24);
                                x = *(a_uint32_t *)(s + 11);
                                *(a_uint32_t *)(d + 8) = (w << 8) | (x >> 24);
                                w = *(a_uint32_t *)(s + 15);
                                *(a_uint32_t *)(d + 12) = (x << 8) | (w >> 24);
                        }
                        break;
                }
        }

        if (n & 16) {
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
        }

        if (n & 8) {
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
        }

        if (n & 4) {
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
        }

        if (n & 2) {
                *d++ = *s++;
                *d++ = *s++;
        }

        if (n & 1) {
                *d = *s;
        }

        return dest;
#endif /* __GNUC__ */

        for (; n; n--) {
                *d++ = *s++;
        }

        return dest;
}
