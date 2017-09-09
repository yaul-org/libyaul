#include <ctype.h>

#undef isblank

int
isblank(int c)
{
        return (_ctype_lookup(c) & _B) || (c == '\t');
}
