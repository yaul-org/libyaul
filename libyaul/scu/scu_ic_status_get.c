/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
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
