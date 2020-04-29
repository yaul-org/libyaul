#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#elif defined(MALLOC_IMPL_SLOB)
#include <mm/slob.h>
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

#include <internal.h>

void * __weak
malloc(size_t n)
{
        void *ret;

#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_GENERAL];

        ret = tlsf_malloc(pool, n);
#elif defined(MALLOC_IMPL_SLOB)
        ret = slob_alloc(n);
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

        return ret;
}
