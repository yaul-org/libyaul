#include <stdio.h>

#include <sys/cdefs.h>

int __weak
getc(FILE *f __unused)
{
        return EOF;
}
