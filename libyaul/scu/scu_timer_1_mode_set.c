/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/timer.h>

#include <scu-internal.h>

void
scu_timer_1_mode_set(bool sp_line)
{

        uint32_t t1md;

        t1md = 0x00000001 | (uint32_t)(sp_line << 8);
        /* Write to memory */
        MEMORY_WRITE(32, SCU(T1MD), t1md);
}
