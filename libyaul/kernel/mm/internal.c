/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#endif /* MALLOC_IMPL_TLSF */

#include <internal.h>

void *
_internal_malloc(size_t n)
{
        void *ret;

#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        ret = tlsf_malloc(pool, n);
#endif /* MALLOC_IMPL_TLSF */

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
#endif /* MALLOC_IMPL_TLSF */

        return ret;
}

void *
_internal_memalign(size_t n, size_t align)
{
        void *ret;

#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        ret = tlsf_memalign(pool, n, align);
#endif /* MALLOC_IMPL_TLSF */

        return ret;
}

void
_internal_free(void *addr)
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        tlsf_free(pool, addr);
#endif /* MALLOC_IMPL_TLSF */
}
