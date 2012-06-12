/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <timer/timer.h>

#include "timer_internal.h"

void
scu_timer_1_set(uint16_t set)
{

        MEM_POKE(TIMER(T1S), set);
}
