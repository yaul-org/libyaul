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

#include <mm/slob.h>

#include <sys/dma-queue.h>

#include <cpu.h>
#include <scu.h>
#include <vdp.h>

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
#include <usb-cart.h>
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#include <arp.h>
#endif /* HAVE_DEV_CARTRIDGE */

#include <dram-cart.h>

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
        slob_init();

        _call_global_ctors();

        cpu_init();
        scu_init();

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
        cpu_intc_mask_set(0);
}

static void __section(".fini") __used __noreturn
_fini(void)
{
        _call_global_dtors();

        while (true) {
        }

        __builtin_unreachable();
}
