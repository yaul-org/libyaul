/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/timer.h>

#include <scu-internal.h>

void
scu_timer_0_set(uint16_t cmp)
{

        MEMORY_WRITE(32, SCU(T0C), cmp);
}
