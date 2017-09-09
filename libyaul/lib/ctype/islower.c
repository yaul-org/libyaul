#include <ctype.h>

#undef islower

int
islower(int c)
{
        return (_ctype_lookup(c) & (_U | _L)) == _L;
}
