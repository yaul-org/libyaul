#include <sys/cdefs.h>

#include <mm/slob.h>

void * __weak
malloc(size_t n)
{
        void *ret;
        ret = slob_alloc(n);

        return ret;
}
