#include <ctype.h>

#undef isprint

int
isprint(int c)
{
        return _ctype_lookup(c) & (_P | _U | _L | _N | _B);
}
