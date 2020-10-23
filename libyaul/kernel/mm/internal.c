/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <mm/tlsf.h>

#include <internal.h>

void
_internal_mm_init(void)
{
        static tlsf_t pools[TLSF_POOL_COUNT];

        master_state()->tlsf_pools = &pools[0];

        master_state()->tlsf_pools[TLSF_POOL_PRIVATE] =
            tlsf_create_with_pool((void *)TLSF_POOL_PRIVATE_START, TLSF_POOL_PRIVATE_SIZE);

#if defined(MALLOC_IMPL_TLSF)
        master_state()->tlsf_pools[TLSF_POOL_USER] =
            tlsf_create_with_pool((void *)TLSF_POOL_USER_START, TLSF_POOL_USER_SIZE);
#else
        master_state()->tlsf_pools[TLSF_POOL_USER] = NULL;
#endif /* MALLOC_IMPL_TLSF */
}

void *
_internal_malloc(size_t n)
{
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        void *ret;
        ret = tlsf_malloc(pool, n);

        return ret;
}

void *
_internal_realloc(void *old, size_t new_len)
{
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        void *ret;
        ret = tlsf_realloc(pool, old, new_len);

        return ret;
}

void *
_internal_memalign(size_t n, size_t align)
{
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        void *ret;
        ret = tlsf_memalign(pool, n, align);

        return ret;
}

void
_internal_free(void *addr)
{
        tlsf_t pool;
        pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        tlsf_free(pool, addr);
}
