/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "usb-cartridge.h"

#include "usb-cartridge_internal.h"

void
usb_cartridge_int(void)
{

        MEM_POKE(32, SCU(ASR0), 0x00000000);
        MEM_POKE(32, SCU(ASR1), 0x00000000);
}
