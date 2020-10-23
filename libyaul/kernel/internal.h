/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_INTERNAL_H_
#define _KERNEL_INTERNAL_H_

#include <sys/cdefs.h>

#include <cpu-internal.h>
#include <dbgio-internal.h>
#include <dram-cart-internal.h>
#include <smpc-internal.h>
#include <vdp-internal.h>

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
#include <usb-cart-internal.h>
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#include <arp.h>
#endif /* HAVE_DEV_CARTRIDGE */

#include <mm/tlsf.h>

#define TLSF_POOL_PRIVATE       (0)
#define TLSF_POOL_PRIVATE_START ((uint32_t)&_end)
#define TLSF_POOL_PRIVATE_END   (TLSF_POOL_PRIVATE_START + 0x8000)
#define TLSF_POOL_PRIVATE_SIZE  (TLSF_POOL_PRIVATE_END - TLSF_POOL_PRIVATE_START)

#define TLSF_POOL_USER          (1)

#ifdef MALLOC_IMPL_TLSF
#define TLSF_POOL_USER_START    (TLSF_POOL_PRIVATE_END)
#define TLSF_POOL_USER_END      (HWRAM(HWRAM_SIZE))
#define TLSF_POOL_USER_SIZE     (TLSF_POOL_USER_END - TLSF_POOL_USER_START)
#endif /* MALLOC_IMPL_TLSF */

#define TLSF_POOL_COUNT         (2)

struct state {
        uint8_t which;

        /* Both master and slave contain their own pools */
        tlsf_t *tlsf_pools;
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

void _internal_mm_init(void);

void *_internal_malloc(size_t);
void *_internal_realloc(void *, size_t);
void *_internal_memalign(size_t, size_t);
void _internal_free(void *);

extern void _internal_dma_queue_init(void);

#endif /* !_KERNEL_INTERNAL_H_ */
