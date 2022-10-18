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
smpc_peripheral_analog_get(smpc_peripheral_t *peripheral,
    smpc_peripheral_analog_t *analog)
{
        assert(peripheral != NULL);
        assert(analog != NULL);

        analog->connected = peripheral->connected;
        analog->port = peripheral->port;
        analog->type = peripheral->type;
        analog->size = peripheral->size;

        (void)memset(&analog->previous.pressed.raw[0], &peripheral->previous_data[0],
            (sizeof(analog->previous.pressed.raw) / sizeof(analog->previous.pressed.raw[0])));
        memset(&analog->pressed.raw[0], &peripheral->data[0],
            (sizeof(analog->pressed.raw) / sizeof(analog->pressed.raw[0])));

        const uint16_t raw = *(uint16_t *)&analog->pressed.raw[0];
        const uint16_t previous_raw = *(uint16_t *)&analog->previous.pressed.raw[0];
        const uint16_t diff = raw ^ previous_raw;

        analog->held.raw = diff & raw;
        analog->released.raw = diff & previous_raw;

        analog->parent = peripheral->parent;
}
