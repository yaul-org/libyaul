#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#endif /* MALLOC_IMPL_TLSF */

#include <internal.h>

void __weak
free(void *addr __unused) /* Keep as __unused */
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_USER];

        tlsf_free(pool, addr);
#else
        assert(false && "Missing implementation. Override malloc symbol");
#endif /* MALLOC_IMPL_TLSF */
}
