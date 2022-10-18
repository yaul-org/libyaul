#include <stdio.h>

#include <sys/cdefs.h>

int
putchar(int c)
{
        return putc(c, stdout);
}
