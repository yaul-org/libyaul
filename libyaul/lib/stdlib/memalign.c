#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#elif defined(MALLOC_IMPL_SLOB)
#include <mm/slob.h>
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

#include <internal.h>

void * __weak
memalign(size_t n, size_t align)
{
        void *ret;

#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_GENERAL];

        ret = tlsf_memalign(pool, n, align);
#elif defined(MALLOC_IMPL_SLOB)
        ret = NULL;
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

        return ret;
}
