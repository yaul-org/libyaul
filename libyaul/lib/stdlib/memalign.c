#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#endif /* MALLOC_IMPL_TLSF */

#include <internal.h>

void * __weak
memalign(size_t n, size_t align)
{
        void *ret;

#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_GENERAL];

        ret = tlsf_memalign(pool, n, align);
#endif /* MALLOC_IMPL_TLSF */

        return ret;
}
