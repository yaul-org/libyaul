/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <timer/timer.h>

#include <scu-internal.h>

void
scu_timer_1_mode_clear(void)
{

        MEMORY_WRITE(32, SCU(T1MD), 0x00000000);
}
