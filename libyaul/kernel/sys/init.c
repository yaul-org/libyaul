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

#include <cpu/cache.h>

#include <cd-block.h>

#include <internal.h>
#include <cpu-internal.h>
#include <dbgio/dbgio-internal.h>
#include <smpc-internal.h>
#include <vdp-internal.h>

void __weak
user_init(void)
{
}

static void __used
_global_ctors_call(void)
{
        extern void (*__CTOR_LIST__[])(void);

        /* Constructors are called in reverse order of the list */
        for (int32_t i = (int32_t)__CTOR_LIST__[0]; i >= 1; i--) {
                /* Each function handles one or more destructor (within file
                 * scope) */
                __CTOR_LIST__[i]();
        }
}

static void __used
_global_dtors_call(void)
{
        extern void (*__DTOR_LIST__[])(void);

        /* Destructors in forward order */
        for (int32_t i = 0; i < (int32_t)__DTOR_LIST__[0]; i++) {
                /* Each function handles one or more destructor (within file
                 * scope) */
                __DTOR_LIST__[i + 1]();
        }
}

static void __used __section(".init")
_init(void)
{
        __mm_init();

        _global_ctors_call();

        __cpu_init();
        __scu_init();
        __smpc_init();
        __smpc_peripheral_init();

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        __usb_cart_init();
#endif /* HAVE_DEV_CARTRIDGE */

        __vdp_init();
        __dbgio_init();

        /* XXX: Fix hard coded value */
        cd_block_init(0x0002);

        cpu_cache_purge();

        user_init();

        cpu_cache_purge();
}

static void __section(".fini") __used __noreturn
_fini(void)
{
        _global_dtors_call();

        while (true) {
        }

        __builtin_unreachable();
}
