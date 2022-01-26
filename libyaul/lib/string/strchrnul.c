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
#include <limits.h>

#define ALIGN           (sizeof(size_t))
#define ONES            ((size_t)-1/UCHAR_MAX)
#define HIGHS           (ONES * (UCHAR_MAX/2+1))
#define HAS_ZERO(x)     (((x)-ONES) & ~(x) & HIGHS)

char *
strchrnul(const char *s, int c)
{
        c = (uint8_t)c;

        if (!c) {
                return (char *)s + strlen(s);
        }

#ifdef __GNUC__
        typedef size_t __may_alias word;
        const word *w;

        for (; (uintptr_t)s % ALIGN; s++) {
                if (!*s || *(uint8_t *)s == c) {
                        return (char *)s;
                }
        }

        size_t k = ONES * c;

        for (w = (void *)s; !HAS_ZERO(*w) && !HAS_ZERO(*w ^ k); w++);

        s = (void *)w;
#endif /* __GNUC__ */

        for (; *s && *(uint8_t *)s != c; s++) {
        }

        return (char *)s;
}
