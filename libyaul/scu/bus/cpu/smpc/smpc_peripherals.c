/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/peripheral.h>

#include <assert.h>
#include <string.h>

#include "smpc-internal.h"

void
smpc_peripheral_analog_get(const smpc_peripheral_t *peripheral,
    smpc_peripheral_analog_t *analog)
{
        assert(peripheral != NULL);
        assert(analog != NULL);

        analog->connected = peripheral->connected;
        analog->port = peripheral->port;
        analog->type = peripheral->type;
        analog->size = peripheral->size;

        /* XXX: Bug? */
        (void)memset(&analog->previous.pressed.raw[0], &peripheral->previous_data[0],
            (sizeof(analog->previous.pressed.raw) / sizeof(analog->previous.pressed.raw[0])));
        /* XXX: Bug? */
        (void)memset(&analog->pressed.raw[0], &peripheral->data[0],
            (sizeof(analog->pressed.raw) / sizeof(analog->pressed.raw[0])));

        const uint16_t raw = *(uint16_t *)&analog->pressed.raw[0];
        const uint16_t previous_raw = *(uint16_t *)&analog->previous.pressed.raw[0];
        const uint16_t diff = raw ^ previous_raw;

        analog->held.raw = diff & raw;
        analog->released.raw = diff & previous_raw;

        analog->parent = peripheral->parent;
}

void
smpc_peripheral_analog_port(uint8_t port, smpc_peripheral_analog_t *analog)
{
        const smpc_peripheral_port_t * const peripheral_port =
            smpc_peripheral_raw_port(port);

        smpc_peripheral_analog_get(peripheral_port->peripheral, analog);
}

void
smpc_peripheral_digital_get(const smpc_peripheral_t *peripheral,
    smpc_peripheral_digital_t *digital)
{
        assert(peripheral != NULL);
        assert(digital != NULL);

        digital->connected = peripheral->connected;
        digital->port = peripheral->port;
        digital->type = peripheral->type;
        digital->size = peripheral->size;

        digital->previous.pressed.raw = *(uint16_t *)&peripheral->previous_data[0];
        digital->pressed.raw = *(uint16_t *)&peripheral->data[0];

        const uint32_t diff = digital->pressed.raw ^ digital->previous.pressed.raw;

        digital->held.raw = diff & digital->pressed.raw;
        digital->released.raw = diff & digital->previous.pressed.raw;

        digital->parent = peripheral->parent;
}

void
smpc_peripheral_digital_port(uint8_t port, smpc_peripheral_digital_t *digital)
{
        const smpc_peripheral_port_t * const peripheral_port =
            smpc_peripheral_raw_port(port);

        smpc_peripheral_digital_get(peripheral_port->peripheral, digital);
}

const smpc_peripheral_keyboard_t *
smpc_peripheral_keyboard_port(uint8_t port)
{
        return (smpc_peripheral_keyboard_t *)smpc_peripheral_raw_port(port);
}

const smpc_peripheral_mouse_t *
smpc_peripheral_mouse_port(uint8_t port)
{
        return (smpc_peripheral_mouse_t *)smpc_peripheral_raw_port(port);
}

const smpc_peripheral_racing_t *
smpc_peripheral_racing_port(uint8_t port)
{
        return (smpc_peripheral_racing_t *)smpc_peripheral_raw_port(port);
}

const smpc_peripheral_port_t *
smpc_peripheral_raw_port(uint8_t port)
{
        assert((port == 1) || (port == 2));

        return &__smpc_peripheral_ports[(port - 1) & 1];
}
