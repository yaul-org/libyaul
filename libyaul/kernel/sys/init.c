/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Joe Fenton <jlfenton65@gmail.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/cdefs.h>

#include <dbgio/dbgio.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#elif defined(MALLOC_IMPL_SLOB)
#include <mm/slob.h>
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

#include <sys/dma-queue.h>

#include <cpu.h>
#include <scu.h>
#include <vdp.h>
#include <smpc.h>

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
#include <usb-cart.h>
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#include <arp.h>
#endif /* HAVE_DEV_CARTRIDGE */

#include <dram-cart.h>

#include <internal.h>

void __weak
user_init(void)
{
}

static void __used
_call_global_ctors(void)
{
        extern void (*__CTOR_LIST__[])(void);

        /* Constructors are called in reverse order of the list */
        int32_t i;
        for (i = (int32_t)__CTOR_LIST__[0]; i >= 1; i--) {
                /* Each function handles one or more destructor (within
                 * file scope) */
                __CTOR_LIST__[i]();
        }
}

/* Do all destructors */
static void __used
_call_global_dtors(void)
{
        extern void (*__DTOR_LIST__[])(void);

        /* Destructors in forward order */
        int32_t i;
        for (i = 0; i < (int32_t)__DTOR_LIST__[0]; i++) {
                /* Each function handles one or more destructor (within
                 * file scope) */
                __DTOR_LIST__[i + 1]();
        }
}

static void __used __section(".init")
_init(void)
{
#if defined(MALLOC_IMPL_TLSF)
        static tlsf_t pools[TLSF_POOL_COUNT];

        master_state()->tlsf_pools = &pools[0];

        master_state()->tlsf_pools[TLSF_POOL_PRIVATE] =
            tlsf_create_with_pool((void *)TLSF_POOL_PRIVATE_START, TLSF_POOL_PRIVATE_SIZE);

        master_state()->tlsf_pools[TLSF_POOL_GENERAL] =
            tlsf_create_with_pool((void *)TLSF_POOL_GENERAL_START, TLSF_POOL_GENERAL_SIZE);
#elif defined(MALLOC_IMPL_SLOB)
        slob_init();
#endif /* MALLOC_IMPL_TLSF || MALLOC_IMPL_SLOB */

        _call_global_ctors();

        cpu_init();
        scu_init();
        smpc_init();
        smpc_peripheral_init();

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        usb_cart_init();
#else
        dram_cart_init();
#endif /* HAVE_DEV_CARTRIDGE */

        dma_queue_init();

        vdp_init();
        dbgio_init();

        user_init();

        cpu_cache_purge();
}

static void __section(".fini") __used __noreturn
_fini(void)
{
        _call_global_dtors();

        while (true) {
        }

        __builtin_unreachable();
}
