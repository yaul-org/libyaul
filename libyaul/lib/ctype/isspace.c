#include <ctype.h>

#undef isspace

int
isspace(int c)
{
        return _ctype_lookup(c) & _S;
}
