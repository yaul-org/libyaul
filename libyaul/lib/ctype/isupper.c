#include <ctype.h>

#undef isupper

int
isupper(int c)
{
        return (_ctype_lookup(c) & (_U | _L)) == _U;
}
