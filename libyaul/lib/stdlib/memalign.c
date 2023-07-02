#include <sys/cdefs.h>

#include <internal.h>

void * __weak
memalign(size_t align, size_t n)
{
    extern void *__user_memalign(size_t align, size_t n);

    return __user_memalign(align, n);
}
