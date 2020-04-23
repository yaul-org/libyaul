/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/peripheral.h>

#include "smpc-internal.h"

smpc_peripheral_keyboard_t *
smpc_peripheral_keyboard_port(uint8_t port)
{
        smpc_peripheral_keyboard_t *port_1;
        smpc_peripheral_keyboard_t *port_2;

        port_1 = (smpc_peripheral_keyboard_t *)smpc_peripheral_port_1.peripheral;
        port_2 = (smpc_peripheral_keyboard_t *)smpc_peripheral_port_2.peripheral;

        switch (port) {
        case 1:
                return port_1;
        case 2:
                return port_2;
        default:
                return NULL;
        }
}
