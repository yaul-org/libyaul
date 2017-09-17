#include <stdint.h>
#include <stddef.h>

#include <mm/slob.h>

void *
malloc(size_t n)
{
        void *ret;
        ret = slob_alloc(n);

        return ret;
}
