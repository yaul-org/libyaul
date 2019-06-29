#include <string.h>

char *
index(const char *s, int c)
{
        return strchr(s, c);
}
