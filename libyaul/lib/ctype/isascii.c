#include <ctype.h>

#undef isascii

int
isascii(int c)
{
        return ((unsigned int)(c) <= 0x7F);
}
