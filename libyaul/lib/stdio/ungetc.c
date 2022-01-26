#include <stdio.h>

#include <sys/cdefs.h>

int __weak
ungetc(int c __unused, FILE *f __unused)
{
        return EOF;
}
