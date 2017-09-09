#include <ctype.h>

#undef isalnum

int
isalnum(int c)
{
        return _ctype_lookup(c) & (_U | _L | _N);
}
