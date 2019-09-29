/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#elif defined(MALLOC_IMPL_SLOB)
#include <mm/slob.h>
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

#include <internal.h>

void *
_internal_malloc(size_t n)
{
        void *ret;

#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        ret = tlsf_malloc(pool, n);
#elif defined(MALLOC_IMPL_SLOB)
        ret = slob_alloc(n);
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

        return ret;
}

void *
_internal_realloc(void *old, size_t new_len)
{
        void *ret;

#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        ret = tlsf_realloc(pool, old, new_len);
#elif defined(MALLOC_IMPL_SLOB)
        ret = slob_realloc(old, new_len);
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

        return ret;
}

void
_internal_free(void *addr)
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        tlsf_free(pool, addr);
#elif defined(MALLOC_IMPL_SLOB)
        slob_free(addr);
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */
}
