/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <smpc/peripheral.h>

#include "smpc-internal.h"

void
smpc_peripheral_analog_port(uint8_t port, smpc_peripheral_analog_t * const peripheral)
{
        smpc_peripheral_t *port_peripheral;

        assert((port == 1) || (port == 2));

        switch (port) {
        default:
        case 1:
                port_peripheral = smpc_peripheral_port_1.peripheral;
                break;
        case 2:
                port_peripheral = smpc_peripheral_port_2.peripheral;
                break;
        }

        smpc_peripheral_analog_get(port_peripheral, peripheral);
}
