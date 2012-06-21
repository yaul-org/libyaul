/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <scu/ic.h>

#include <scu-internal.h>

uint32_t
scu_ic_status_get(void)
{
        uint32_t ist;

        ist = MEMORY_READ(32, SCU(IST));
        return ist;
}
