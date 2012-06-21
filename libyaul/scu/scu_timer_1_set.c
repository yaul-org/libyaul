/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <scu/timer.h>

#include <scu-internal.h>

void
scu_timer_1_set(uint16_t set)
{

        MEMORY_WRITE(32, SCU(T1S), set);
}
