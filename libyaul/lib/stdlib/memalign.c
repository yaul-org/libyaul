#include <sys/cdefs.h>

#include <internal.h>

void * __weak
memalign(size_t n, size_t align)
{
        extern void *__user_memalign(size_t n, size_t align);

        return __user_memalign(n, align);
}
