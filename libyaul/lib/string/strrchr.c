#include <string.h>

char *strrchr(const char *s, int c)
{
        return memrchr(s, c, strlen(s) + 1);
}
