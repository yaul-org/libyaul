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

#include <stdio.h>

#include <sys/cdefs.h>

int
fseek(FILE *f, long off, int whence)
{
        /* Adjust relative offset for unread data in buffer, if any */
        if ((whence == SEEK_CUR) && (f->rend != NULL)) {
                off -= f->rend - f->rpos;
        }

        /* Flush write buffer, and report error on failure */
        if (f->wpos != f->wbase) {
                f->write(f, 0, 0);

                if (f->wpos == NULL) {
                        return -1;
                }
        }

        /* Leave writing mode */
        f->wpos = f->wbase = f->wend = 0;

        /* Perform the underlying seek. */
        if (f->seek(f, off, whence) < 0) {
                return -1;
        }

        /* If seek succeeded, file is seekable and we discard read buffer */
        f->rpos = f->rend = 0;
        f->flags &= ~F_EOF;

        return 0;
}
