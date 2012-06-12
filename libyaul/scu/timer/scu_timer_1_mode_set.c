/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <timer/timer.h>

#include "timer_internal.h"

void
scu_timer_1_mode_set(bool sp_line)
{

        MEM_POKE(TIMER(T1MD), 0x00000001 | ((uint16_t)sp_line << 8));
}
