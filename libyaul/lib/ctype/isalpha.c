#include <ctype.h>

#undef isalpha

int
isalpha(int c)
{
        return _ctype_lookup(c) & (_U | _L);
}
