#include <stdint.h>
#include <string.h>

char *strchr(const char *s, int c)
{
        char *r = __strchrnul(s, c);

        return *(uint8_t *)r == (uint8_t)c ? r : 0;
}
