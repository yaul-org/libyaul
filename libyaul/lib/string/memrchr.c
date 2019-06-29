#include <string.h>
#include <stdint.h>

void *
memrchr(const void *m, int c, size_t n)
{
        const uint8_t *s = m;
        c = (uint8_t)c;

        while (n--) {
                if (s[n] == c) {
                        return (void *)(s + n);
                }
        }

        return 0;
}
