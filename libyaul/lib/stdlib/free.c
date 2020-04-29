#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#elif defined(MALLOC_IMPL_SLOB)
#include <mm/slob.h>
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

#include <internal.h>

void __weak
free(void *addr)
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_GENERAL];

        tlsf_free(pool, addr);
#elif defined(MALLOC_IMPL_SLOB)
        slob_free(addr);
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */
}
