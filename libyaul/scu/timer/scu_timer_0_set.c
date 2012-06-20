/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <timer/timer.h>

#include <scu-internal.h>

void
scu_timer_0_set(uint16_t cmp)
{

        MEMORY_WRITE(32, SCU(T0C), cmp);
}
