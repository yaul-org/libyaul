#include <ctype.h>

#undef isxdigit

int
isxdigit(int c)
{
        return _ctype_lookup(c) & (_X | _N);
}
