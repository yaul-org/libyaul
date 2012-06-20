/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen 
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <usb-cartridge.h>

#include "usb-cartridge-internal.h"

#define USB_TXE         0x02

void
usb_cartridge_send_byte(uint8_t c)
{

        while ((MEMORY_READ(8, USB_CARTRIDGE(FLAG)) & USB_TXE));
        MEMORY_WRITE(8, USB_CARTRIDGE(FIFO), c);
}
