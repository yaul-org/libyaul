#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#endif /* MALLOC_IMPL_TLSF */

#include <internal.h>

void * __weak
memalign(size_t n __unused, size_t align __unused) /* Keep as __unused */
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_USER];

        void *ret;
        ret = tlsf_memalign(pool, align, n);

        return ret;
#else
        assert(false && "Missing implementation. Override memalign symbol");
#endif /* MALLOC_IMPL_TLSF */
}
