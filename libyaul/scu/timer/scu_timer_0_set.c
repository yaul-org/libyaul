/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <timer/timer.h>

#include "timer_internal.h"

void
scu_timer_0_set(uint16_t cmp)
{

        MEM_POKE(TIMER(T0C), cmp);
}
