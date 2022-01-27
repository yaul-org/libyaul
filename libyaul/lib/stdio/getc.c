#include <stdio.h>

#include <sys/cdefs.h>

int
getc(FILE *f)
{
        if (f->rpos != f->rend) {
                return *f->rpos++;
        }

        unsigned char c;

        if ((f->read(f, &c, 1)) == 1) {
                return c;
        }

        return EOF;
}
