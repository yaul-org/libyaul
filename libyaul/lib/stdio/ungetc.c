#include <stdio.h>

#include <sys/cdefs.h>

int
ungetc(int c, FILE *f)
{
        if (c == EOF) {
                return c;
        }

        if ((f->rpos == NULL) || (f->rpos <= (f->buf - UNGET))) {
                return EOF;
        }

        *--f->rpos = c;

        f->flags &= ~F_EOF;

        return (unsigned char)c;
}
