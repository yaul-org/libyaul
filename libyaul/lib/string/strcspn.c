#include <string.h>
#include <stdint.h>

#define BITOP(a,b,op)                                                          \
    ((a)[(size_t)(b) / (8 * sizeof *(a))] op (size_t)1 << ((size_t)(b) % (8 * sizeof *(a))))

size_t
strcspn(const char *s, const char *c)
{
        const char *a = s;
        size_t byteset[32 / sizeof(size_t)];

        if (!c[0] || !c[1]) {
                return strchrnul(s, *c) - a;
        }

        memset(byteset, 0, sizeof byteset);

        for (; *c && BITOP(byteset, *(uint8_t *)c, |= ); c++);

        for (; *s && !BITOP(byteset, *(uint8_t *)s, &); s++);

        return s - a;
}
