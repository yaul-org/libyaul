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
#include <stdio.h>

#include <sys/cdefs.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

size_t
fread(void * restrict destv, size_t size, size_t nmemb, FILE * restrict f)
{
        if (size == 0) {
                nmemb = 0;
        }

        const size_t len = size * nmemb;

        unsigned char *dest = destv;
        size_t l = len;
        size_t k;

        if (f->rpos != f->rend) {
                /* First exhaust the buffer */
                k = MIN((size_t)(f->rend - f->rpos), l);

                (void)memcpy(dest, f->rpos, k);

                f->rpos += k;
                dest += k;
                l -= k;
        }

        /* Read the remainder directly */
        for (; l; l -= k, dest += k) {
                k = f->read(f, dest, l);

                if (k == 0) {
                        return ((len - l) / size);
                }
        }

        return nmemb;
}
