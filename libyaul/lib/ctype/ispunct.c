#include <ctype.h>

#undef ispunct

int
ispunct(int c)
{
        return _ctype_lookup(c) & _P;
}
