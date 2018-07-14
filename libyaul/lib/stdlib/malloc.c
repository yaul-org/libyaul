#include <mm/slob.h>

void * __attribute__ ((weak))
malloc(size_t n)
{
        void *ret;
        ret = slob_alloc(n);

        return ret;
}
