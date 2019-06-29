#include <string.h>
#include <stdint.h>
#include <limits.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

char *__strchrnul(const char *s, int c)
{
        c = (uint8_t)c;
        if (!c) return (char *)s + strlen(s);

#ifdef __GNUC__
        typedef size_t __may_alias word;
        const word *w;
        for (; (uintptr_t)s % ALIGN; s++)
                if (!*s || *(uint8_t *)s == c) return (char *)s;
        size_t k = ONES * c;
        for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
        s = (void *)w;
#endif /* __GNUC__ */

        for (; *s && *(uint8_t *)s != c; s++);

        return (char *)s;
}

__weak_alias(__strchrnul, strchrnul);
