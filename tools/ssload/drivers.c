/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include "drivers.h"

extern const struct device_driver device_datalink;
extern const struct device_driver device_usb_cartridge;

const struct device_driver *device_drivers[] = {
        &device_datalink,
        &device_usb_cartridge
};
