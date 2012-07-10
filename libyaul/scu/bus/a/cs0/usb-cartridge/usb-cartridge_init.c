/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include "usb-cartridge-internal.h"

void
usb_cartridge_init(void)
{

        MEMORY_WRITE(32, SCU(ASR0), 0x00000000);
        MEMORY_WRITE(32, SCU(ASR1), 0x00000000);
}
