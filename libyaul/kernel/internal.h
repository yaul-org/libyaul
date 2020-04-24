/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_INTERNAL_H_
#define _KERNEL_INTERNAL_H_

#include <sys/cdefs.h>

#ifdef MALLOC_IMPL_TLSF
#include <mm/tlsf.h>

#define TLSF_POOL_PRIVATE       (0)
#define TLSF_POOL_PRIVATE_START ((uint32_t)&_end)
#define TLSF_POOL_PRIVATE_END   (TLSF_POOL_PRIVATE_START + 0x10000)
#define TLSF_POOL_PRIVATE_SIZE  (TLSF_POOL_PRIVATE_END - TLSF_POOL_PRIVATE_START)

#define TLSF_POOL_GENERAL       (1)
#define TLSF_POOL_GENERAL_START (TLSF_POOL_PRIVATE_END)
#define TLSF_POOL_GENERAL_END   (HWRAM(0) + HWRAM_SIZE)
#define TLSF_POOL_GENERAL_SIZE  (TLSF_POOL_GENERAL_END - TLSF_POOL_GENERAL_START)

#define TLSF_POOL_COUNT         (2)
#endif /* MALLOC_IMPL_TLSF */

struct state {
        char which;

#ifdef MALLOC_IMPL_TLSF
        /* Both master and slave contain their own pools */
        tlsf_t *tlsf_pools;
#endif /* MALLOC_IMPL_TLSF */
};

static inline struct state * __always_inline
master_state(void)
{
        extern struct state _internal_master_state;

        return &_internal_master_state;
}

static inline struct state * __always_inline
slave_state(void)
{
        extern struct state _internal_slave_state;

        return &_internal_slave_state;
}

extern void *_end;

void _internal_reset(void);

void *_internal_malloc(size_t);
void *_internal_realloc(void *, size_t);
void *_internal_memalign(size_t, size_t);
void _internal_free(void *);

#endif /* !_KERNEL_INTERNAL_H_ */
