/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include <common.h>

#include <stdbool.h>

#include "usb-cartridge-internal.h"

void __noreturn
usb_cartridge_return(void)
{
        ((void (*)(void))0x0203F800)();

        while (true) {
        }
}
