#include <ctype.h>

#undef iscntrl

int
iscntrl(int c)
{
        return _ctype_lookup(c) & _C;
}
