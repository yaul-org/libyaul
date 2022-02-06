#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#endif /* MALLOC_IMPL_TLSF */

#include <internal.h>

void * __weak
malloc(size_t n)
{
        extern void *__user_malloc(size_t n);

        return __user_malloc(n);
}
