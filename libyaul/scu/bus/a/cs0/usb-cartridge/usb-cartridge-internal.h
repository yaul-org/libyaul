/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _USB_CARTRIDGE_INTERNAL_H_
#define _USB_CARTRIDGE_INTERNAL_H_

#include <scu-internal.h>

/* Macros specific for processor */
#define USB_CARTRIDGE(x) (0x22000001 + ((x) << 20))

#define FIFO            0x01
#define FLAG            0x02

#endif /* !_USB_CARTRIDGE_INTERNAL_H_ */
