/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Anders Montonen 
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include "usb-cartridge-internal.h"

uint32_t
usb_cartridge_long_read(void)
{
        uint32_t b;

        b = 0;

        b |= (usb_cartridge_byte_read()) << 24;
        b |= (usb_cartridge_byte_read()) << 16;
        b |= (usb_cartridge_byte_read()) << 8;
        b |= (usb_cartridge_byte_read());

        return b;
}
