/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "usb-cartridge.h"

#include "usb-cartridge_internal.h"

#define USB_RXF         0x01

uint8_t
usb_cartridge_read_byte(void)
{
        uint8_t b;

        while ((MEM_READ(8, USB_CARTRIDGE(FLAG)) & USB_RXF));
        b = MEM_READ(8, USB_CARTRIDGE(FIFO));

        return b;
}
