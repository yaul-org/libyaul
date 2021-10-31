#include <sys/cdefs.h>

#include <internal.h>

void * __weak
memalign(size_t n, size_t align)
{
        extern void *_internal_user_memalign(size_t n, size_t align);

        return _internal_user_memalign(n, align);
}
