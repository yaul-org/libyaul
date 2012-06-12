/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "dram-cartridge_internal.h"

#include "dram-cartridge.h"

void *
dram_cartridge_area(void)
{
        void *addr;

        addr = (void *)((id != 0x00000000)
            ? CS0(0x00000000)
            : 0x00000000);

        return addr;
}
