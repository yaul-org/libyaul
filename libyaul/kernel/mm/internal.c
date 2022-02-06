/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <mm/tlsf.h>
#include <mm/mm_stats.h>

#include <internal.h>

#define TLSF_HANDLE_PRIVATE       (0)
#define TLSF_HANDLE_USER          (1)
#define TLSF_HANDLE_COUNT         (2)

#define TLSF_POOL_PRIVATE_START ((uint32_t)&_private_pool[0])
#define TLSF_POOL_PRIVATE_SIZE  (0xA000)

#define TLSF_POOL_USER_START    ((uint32_t)&_end)
#define TLSF_POOL_USER_END      (HWRAM(HWRAM_SIZE))
#define TLSF_POOL_USER_SIZE     (TLSF_POOL_USER_END - TLSF_POOL_USER_START)

static uint8_t _private_pool[TLSF_POOL_PRIVATE_SIZE];

static tlsf_t _handles[TLSF_HANDLE_COUNT];

static mm_stats_walker_t _current_stats_walker;

static void _stats_walker(tlsf_t tlsf, mm_stats_walker_t walker, void *work);

static void _default_stats_walker(const mm_stats_walk_entry_t *walk_entry);

static void _tlsf_walker(void *ptr, size_t size, int used, void *user);

void
__mm_init(void)
{
        master_state()->tlsf_handles = &_handles[0];

        master_state()->tlsf_handles[TLSF_HANDLE_PRIVATE] =
            tlsf_pool_create((void *)TLSF_POOL_PRIVATE_START, TLSF_POOL_PRIVATE_SIZE);

#if defined(MALLOC_IMPL_TLSF)
        master_state()->tlsf_handles[TLSF_HANDLE_USER] =
            tlsf_pool_create((void *)TLSF_POOL_USER_START, TLSF_POOL_USER_SIZE);
#else
        master_state()->tlsf_pools[TLSF_HANDLE_USER] = NULL;
#endif /* MALLOC_IMPL_TLSF */
}

void *
__malloc(size_t n)
{
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_PRIVATE];

        return tlsf_malloc(handle, n);
}

void *
__realloc(void *old, size_t new_len)
{
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_PRIVATE];

        return tlsf_realloc(handle, old, new_len);
}

void *
__memalign(size_t n, size_t align)
{
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_PRIVATE];

        return tlsf_memalign(handle, n, align);
}

void
__free(void *addr)
{
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_PRIVATE];

        tlsf_free(handle, addr);
}

void *
__user_malloc(size_t n __unused) /* Keep as __unused */
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_USER];

        return tlsf_malloc(handle, n);
#endif /* MALLOC_IMPL_TLSF */
}

void *
__user_realloc(void *old __unused, size_t new_len __unused) /* Keep as __unused */
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_USER];

        return tlsf_realloc(handle, old, new_len);
#endif /* MALLOC_IMPL_TLSF */
}

void *
__user_memalign(size_t n __unused, size_t align __unused) /* Keep as __unused */
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_USER];

        return tlsf_memalign(handle, align, n);
#endif /* MALLOC_IMPL_TLSF */
}

void
__user_free(void *addr __unused) /* Keep as __unused */
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_USER];

        tlsf_free(handle, addr);
#else
        assert(false && "Missing implementation. Override malloc symbol");
#endif /* MALLOC_IMPL_TLSF */
}

void
__mm_stats_walk(mm_stats_walker_t walker __unused, void *work __unused) /* Keep as __unused */
{
#if defined(MALLOC_IMPL_TLSF)
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_USER];

        _stats_walker(handle, walker, work);
#endif /* MALLOC_IMPL_TLSF */
}

void
__mm_stats_private_walk(mm_stats_walker_t walker, void *work)
{
        tlsf_t const handle = master_state()->tlsf_handles[TLSF_HANDLE_PRIVATE];

        _stats_walker(handle, walker, work);
}

static void
_stats_walker(tlsf_t tlsf, mm_stats_walker_t walker, void *work)
{
        tlsf_pool_t const pool = tlsf_pool_get(tlsf);

        _current_stats_walker = (walker != NULL) ? walker : _default_stats_walker;

        mm_stats_walk_entry_t walk_entry = {
                .work = work
        };

        tlsf_pool_walk(pool, _tlsf_walker, &walk_entry);
}

static void
_default_stats_walker(const mm_stats_walk_entry_t *walk_entry __unused)
{
}

static void
_tlsf_walker(void *ptr, size_t size, int used, void *user)
{
        mm_stats_walk_entry_t * const walk_entry = user;

        walk_entry->address = (uintptr_t)ptr;
        walk_entry->size = size;
        walk_entry->used = (bool)used;

        _current_stats_walker(walk_entry);
}
