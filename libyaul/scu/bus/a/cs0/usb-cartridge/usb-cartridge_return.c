/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include "usb-cartridge-internal.h"

void
usb_cartridge_return(void)
{
        ((void (*)(void))0x0203F800)();
}
