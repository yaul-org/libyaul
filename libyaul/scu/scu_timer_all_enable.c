/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/timer.h>

#include <scu-internal.h>

void
scu_timer_all_enable(void)
{
        uint32_t t1md;

        t1md = MEMORY_READ(32, SCU(T1MD));
        t1md |= 0x00000001;
        /* Write to memory */
        MEMORY_WRITE(32, SCU(T1MD), t1md);
}
