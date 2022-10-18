/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/peripheral.h>

#include "smpc-internal.h"

const smpc_peripheral_keyboard_t *
smpc_peripheral_keyboard_port(uint8_t port)
{
        switch (port) {
        case 1:
                return (smpc_peripheral_keyboard_t *)__smpc_peripheral_port_1.peripheral;
        case 2:
                return (smpc_peripheral_keyboard_t *)__smpc_peripheral_port_2.peripheral;
        default:
                return NULL;
        }
}
