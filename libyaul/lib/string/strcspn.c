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

#define BITOP(a,b,op) ((a)[(size_t)(b) / (8 * sizeof *(a))] op (size_t)1 << ((size_t)(b) % (8 * sizeof *(a))))

size_t
strcspn(const char *s, const char *c)
{
        const char *a = s;
        size_t byteset[32 / sizeof(size_t)];

        if (!c[0] || !c[1]) {
                return strchrnul(s, *c) - a;
        }

        memset(byteset, 0, sizeof byteset);

        for (; *c && BITOP(byteset, *(uint8_t *)c, |= ); c++) {
        }

        for (; *s && !BITOP(byteset, *(uint8_t *)s, &); s++) {
        }

        return s - a;
}
