/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/peripheral.h>

#include <assert.h>

#include "smpc-internal.h"

void
smpc_peripheral_digital_port(uint8_t port, struct smpc_peripheral_digital * const peripheral)
{
        struct smpc_peripheral *port_peripheral;

        assert((port == 1) || (port == 2));

        switch (port) {
        case 1:
                port_peripheral = smpc_peripheral_port_1.peripheral;
                break;
        case 2:
                port_peripheral = smpc_peripheral_port_2.peripheral;
                break;
        }

        peripheral->connected = port_peripheral->connected;
        peripheral->port = port_peripheral->port;
        peripheral->type = port_peripheral->type;
        peripheral->size = port_peripheral->size;

        peripheral->previous.pressed.raw = *(uint16_t *)&port_peripheral->previous_data[0];
        peripheral->current.pressed.raw = *(uint16_t *)&port_peripheral->data[0];

        uint32_t diff;

        diff = peripheral->current.pressed.raw ^ peripheral->previous.pressed.raw;

        peripheral->current.held.raw = diff & peripheral->current.pressed.raw;
        peripheral->current.released.raw = diff & peripheral->previous.pressed.raw;

        peripheral->parent = port_peripheral->parent;
}
