/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <mm/tlsf.h>

#include <internal.h>

#define TLSF_POOL_PRIVATE_START ((uint32_t)&_private_pool[0])
#define TLSF_POOL_PRIVATE_SIZE  (0x8000)

#define TLSF_POOL_USER_START    ((uint32_t)&_end)
#define TLSF_POOL_USER_END      (HWRAM(HWRAM_SIZE))
#define TLSF_POOL_USER_SIZE     (TLSF_POOL_USER_END - TLSF_POOL_USER_START)

static uint8_t _private_pool[TLSF_POOL_PRIVATE_SIZE];

static tlsf_t _pools[TLSF_POOL_COUNT];

void
_internal_mm_init(void)
{
        master_state()->tlsf_pools = &_pools[0];

        master_state()->tlsf_pools[TLSF_POOL_PRIVATE] =
            tlsf_pool_create((void *)TLSF_POOL_PRIVATE_START, TLSF_POOL_PRIVATE_SIZE);

#if defined(MALLOC_IMPL_TLSF)
        master_state()->tlsf_pools[TLSF_POOL_USER] =
            tlsf_pool_create((void *)TLSF_POOL_USER_START, TLSF_POOL_USER_SIZE);
#else
        master_state()->tlsf_pools[TLSF_POOL_USER] = NULL;
#endif /* MALLOC_IMPL_TLSF */
}

void *
_internal_malloc(size_t n)
{
        tlsf_t const pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        void * const ret = tlsf_malloc(pool, n);

        return ret;
}

void *
_internal_realloc(void *old, size_t new_len)
{
        tlsf_t const pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        void * const ret = tlsf_realloc(pool, old, new_len);

        return ret;
}

void *
_internal_memalign(size_t n, size_t align)
{
        tlsf_t const pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        void * const ret = tlsf_memalign(pool, n, align);

        return ret;
}

void
_internal_free(void *addr)
{
        tlsf_t const pool = master_state()->tlsf_pools[TLSF_POOL_PRIVATE];

        tlsf_free(pool, addr);
}
