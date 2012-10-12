#include <inttypes.h>
#include <stddef.h>

#include "slob.h"

void *
malloc(size_t n)
{
        void *ret;

        ret = slob_alloc(n);

        return ret;
}
