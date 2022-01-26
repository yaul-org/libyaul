#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include <sys/cdefs.h>

int __weak
vfprintf(FILE * restrict f __unused, const char * restrict fmt __unused, va_list ap __unused)
{
        return 0;
}
