/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include "usb-cartridge-internal.h"

#define USB_RXF         0x01

uint8_t
usb_cartridge_read_byte(void)
{
        uint8_t b;

        while ((MEMORY_READ(8, USB_CARTRIDGE(FLAG)) & USB_RXF));
        b = MEMORY_READ(8, USB_CARTRIDGE(FIFO));

        return b;
}
