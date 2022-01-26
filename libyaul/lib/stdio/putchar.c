#include <stdio.h>

#include <sys/cdefs.h>

int __weak
putchar(int c __unused)
{
        return EOF;
}
