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
#include <dbgio/dbgio-internal.h>
#include <dram-cart-internal.h>
#include <mm/memb-internal.h>
#include <smpc-internal.h>
#include <vdp-internal.h>

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
#include <usb-cart-internal.h>
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#include <arp.h>
#endif /* HAVE_DEV_CARTRIDGE */

#include <mm/tlsf.h>

struct state {
        uint8_t which;

        /* Both master and slave contain their own set of handles */
        tlsf_t *tlsf_handles;
};

static inline struct state * __always_inline
master_state(void)
{
        extern struct state __master_state;

        return &__master_state;
}

static inline struct state * __always_inline
slave_state(void)
{
        extern struct state __slave_state;

        return &__slave_state;
}

extern void *_end;

void __reset(void);

void __mm_init(void);

void *__malloc(size_t);
void *__realloc(void *, size_t);
void *__memalign(size_t, size_t);
void __free(void *);

extern void __dma_queue_init(void);

#endif /* !_KERNEL_INTERNAL_H_ */
