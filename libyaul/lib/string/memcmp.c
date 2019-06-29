#include <string.h>
#include <stdint.h>

int memcmp(const void *vl, const void *vr, size_t n)
{
        const uint8_t *l=vl, *r=vr;
        for (; n && *l == *r; n--, l++, r++);
        return n ? *l-*r : 0;
}
