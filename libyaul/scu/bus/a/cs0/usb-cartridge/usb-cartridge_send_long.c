/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "usb-cartridge.h"

#include "usb-cartridge_internal.h"

void
usb_cartridge_send_long(uint32_t w)
{

        usb_cartridge_send_byte((w >> 24));
        usb_cartridge_send_byte((w >> 16));
        usb_cartridge_send_byte((w >> 8));
        usb_cartridge_send_byte((w & 0xFF));
}
