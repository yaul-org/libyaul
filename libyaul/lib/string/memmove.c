#include <string.h>
#include <stdint.h>

#ifdef __GNUC__
typedef __may_alias size_t WT;
#define WS (sizeof(WT))
#endif /* __GNUC__ */

void *memmove(void *dest, const void *src, size_t n)
{
        char *d = dest;
        const char *s = src;

        if (d==s) return d;
        if ((uintptr_t)s-(uintptr_t)d-n <= -2*n) return memcpy(d, s, n);

        if (d<s) {
#ifdef __GNUC__
                if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
                        while ((uintptr_t)d % WS) {
                                if (!n--) return dest;
                                *d++ = *s++;
                        }
                        for (; n>=WS; n-=WS, d+=WS, s+=WS) *(WT *)d = *(WT *)s;
                }
#endif /* __GNUC__ */
                for (; n; n--) *d++ = *s++;
        } else {
#ifdef __GNUC__
                if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
                        while ((uintptr_t)(d+n) % WS) {
                                if (!n--) return dest;
                                d[n] = s[n];
                        }
                        while (n>=WS) n-=WS, *(WT *)(d+n) = *(WT *)(s+n);
                }
#endif /* __GNUC__ */
                while (n) n--, d[n] = s[n];
        }

        return dest;
}
