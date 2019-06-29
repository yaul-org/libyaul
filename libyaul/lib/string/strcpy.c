#include <string.h>

char *strcpy(char *restrict dst, const char *restrict src)
{
        stpcpy(dst, src);

        return dst;
}
