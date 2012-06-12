/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "usb-cartridge.h"

#include "usb-cartridge_internal.h"

uint8_t
usb_cartridge_xchg_byte(uint8_t c)
{
        uint8_t b;

        b = usb_cartridge_read_byte();
        usb_cartridge_send_byte(c);

        return b;
}
