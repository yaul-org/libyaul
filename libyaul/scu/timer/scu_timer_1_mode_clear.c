/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <timer/timer.h>

#include "timer_internal.h"

void
scu_timer_1_mode_clear(void)
{

        MEM_POKE(TIMER(T1MD), 0x00000000);
}
