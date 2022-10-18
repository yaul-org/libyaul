#include <stdio.h>

#include <sys/cdefs.h>

int
putc(int c, FILE *f)
{
        const unsigned char buf[] = {
                c
        };

        if (f->wpos != f->wend) {
                *f->wpos++ = c;
        } else if ((f->write(f, buf, 1)) != 1) {
                c = EOF;
        }

        return c;
}
