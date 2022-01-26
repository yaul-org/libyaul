#include <stdio.h>

#include <sys/cdefs.h>

int __weak
rename(const char *old __unused, const char *new __unused)
{
        return 0;
}
