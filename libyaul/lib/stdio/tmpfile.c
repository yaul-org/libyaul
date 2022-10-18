#include <sys/cdefs.h>

#include <stdio.h>

FILE * __weak
tmpfile(void)
{
        return NULL;
}
