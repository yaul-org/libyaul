#include <stdio.h>

#include <sys/cdefs.h>

int __weak
setvbuf(FILE *restrict f __unused, char *restrict buf __unused, int type __unused,
    size_t size __unused)
{
        return -1;
}
