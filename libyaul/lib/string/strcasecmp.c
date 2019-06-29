#include <ctype.h>

int strcasecmp(const char *_l, const char *_r)
{
        const uint8_t *l = (void *)_l, *r = (void *)_r;

        for (; *l && *r && (*l == *r || tolower(*l) == tolower(*r)); l++, r++);

        return tolower(*l) - tolower(*r);
}
