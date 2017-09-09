#include <ctype.h>

#undef isgraph

int
isgraph(int c)
{
        return _ctype_lookup(c) & (_P | _U | _L | _N);
}
