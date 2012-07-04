/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include "usb-cartridge-internal.h"

uint32_t
usb_cartridge_read_long(void)
{
        uint32_t b;

        b = 0;

        b |= (usb_cartridge_read_byte()) << 24;
        b |= (usb_cartridge_read_byte()) << 16;
        b |= (usb_cartridge_read_byte()) << 8;
        b |= (usb_cartridge_read_byte());

        return b;
}
