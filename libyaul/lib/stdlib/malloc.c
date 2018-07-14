#include <stdint.h>
#include <stddef.h>

#include <mm/slob.h>

void * __attribute__ ((weak, alias("malloc")))
override_malloc(size_t n)
{
        void *ret;
        ret = slob_alloc(n);

        return ret;
}

void *
malloc(size_t n)
{
        return override_malloc(n);
}
