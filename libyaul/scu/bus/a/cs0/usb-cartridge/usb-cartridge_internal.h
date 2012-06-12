/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _USB_CARTRIDGE_INTERNAL_H_
#define _USB_CARTRIDGE_INTERNAL_H_

#include <inttypes.h>

/* Write and read directly to specified address */
#define MEM_POKE(t, x, y)       (*(volatile uint ## t ## _t *)(x) = (y))
#define MEM_READ(t, x)          (*(volatile uint ## t ## _t *)(x))

/* Macros specific for processor */
#define SCU(x)                  (0x25FE0000 + (x))
#define USB_CARTRIDGE(x)        (0x22000001 + ((x) << 20))

enum usb_cartridge_regs {
        ASR0 = 0xB0,
        ASR1 = 0xB4,
        AREF = 0xB8,
        FIFO = 0x01,
        FLAG = 0x02
};

#endif /* !_USB_CARTRIDGE_INTERNAL_H_ */
