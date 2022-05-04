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

static void __used __section(".init")
_init(void)
{
        __atexit_init();

        __mm_init();

        __cpu_init();
        __scu_init();
        __smpc_init();

        __vdp_init();

        cpu_cache_purge();

        user_init();

        cpu_cache_purge();
}

static void __section(".fini") __used __noreturn
_fini(void)
{
        exit(0);
}
