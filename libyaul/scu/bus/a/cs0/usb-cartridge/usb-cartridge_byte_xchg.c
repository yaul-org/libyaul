/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include "usb-cartridge-internal.h"

uint8_t
usb_cartridge_byte_xchg(uint8_t c)
{
        uint8_t b;

        b = usb_cartridge_byte_read();
        usb_cartridge_byte_send(c);

        return b;
}
