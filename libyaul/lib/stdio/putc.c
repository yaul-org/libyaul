#include <stdio.h>

#include <sys/cdefs.h>

int __weak
putc(int c __unused, FILE *f __unused)
{
        return EOF;
}
