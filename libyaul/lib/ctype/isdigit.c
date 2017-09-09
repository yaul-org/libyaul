#include <ctype.h>

#undef isdigit

int
isdigit(int c)
{
        return _ctype_lookup(c) & _N;
}
