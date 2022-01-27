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
#include <stdio.h>

#include <sys/cdefs.h>

off_t
__ftell(FILE *f)
{
        const int whence =
            ((f->flags & F_APP) && (f->wpos != f->wbase))
            ? SEEK_END : SEEK_CUR;

        off_t pos = f->seek(f, 0, whence);

        if (pos < 0) {
                return pos;
        }

        /* Adjust for data in buffer */
        if (f->rend != NULL) {
                pos += f->rpos - f->rend;
        } else if (f->wbase) {
                pos += f->wpos - f->wbase;
        }

        return pos;
}

long
ftell(FILE *f)
{
        off_t pos = __ftell(f);

        if (pos > LONG_MAX) {
                errno = EOVERFLOW;
                return -1;
        }

        return pos;
}
