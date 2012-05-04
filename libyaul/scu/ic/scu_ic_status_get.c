/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <ic/ic.h>

#include "ic_internal.h"

uint32_t
scu_ic_status_get(void)
{
        uint32_t ist;

        ist = MEM_READ(IC(IST));
        return ist;
}
