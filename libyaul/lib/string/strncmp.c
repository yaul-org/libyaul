#include <string.h>
#include <stdint.h>

int
strncmp(const char *_l, const char *_r, size_t n)
{
        const uint8_t *l = (void *)_l, *r = (void *)_r;

        if (!n--) {
                return 0;
        }

        for (; *l && *r && n && *l == *r ; l++, r++, n--);

        return *l - *r;
}
