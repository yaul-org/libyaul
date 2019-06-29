#include <string.h>
#include <stdint.h>

int
strcmp(const char *l, const char *r)
{
        for (; *l == *r && *l; l++, r++);

        return *(uint8_t *)l - *(uint8_t *)r;
}
