/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/ic.h>
#include <smpc/peripheral.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lib/memb.h>

#include <sys/queue.h>

#include "smpc-internal.h"

uint8_t offset = 0;

struct smpc_peripheral_port smpc_peripheral_port_1;
struct smpc_peripheral_port smpc_peripheral_port_2;

static struct smpc_peripheral *peripheral_alloc(void);
static void peripheral_free(struct smpc_peripheral *);

static int peripheral_update(struct smpc_peripheral *, uint8_t);

MEMB(peripherals, struct smpc_peripheral, 2, 2);

void
smpc_peripheral_parse_init(void)
{
        memb_init(&peripherals);

        smpc_peripheral_port_1.peripheral = peripheral_alloc();
        TAILQ_INIT(&smpc_peripheral_port_1.peripherals);

        smpc_peripheral_port_2.peripheral = peripheral_alloc();
        TAILQ_INIT(&smpc_peripheral_port_2.peripherals);
}

void
smpc_peripheral_parse(irq_mux_handle_t *hdl __attribute__ ((unused)))
{
        uint32_t ist;

        /* Could this help? */
        ist = scu_ic_status_get();
        if ((ist & IC_IST_SYSTEM_MANAGER) == IC_IST_SYSTEM_MANAGER) {
                return;
        }

        struct smpc_peripheral_port *port;
        struct smpc_peripheral_port *ports[] = {
                &smpc_peripheral_port_1,
                &smpc_peripheral_port_2,
                NULL
        };

        int port_idx;

        for (port_idx = 0; ports[port_idx] != NULL; port_idx++) {
                port = ports[port_idx];

                struct smpc_peripheral *peripheral;
                struct smpc_peripheral *tmp_peripheral;

                for (peripheral = TAILQ_FIRST(&port->peripherals);
                     peripheral != NULL;
                     peripheral = tmp_peripheral) {
                        tmp_peripheral = TAILQ_NEXT(peripheral, peripherals);
                        TAILQ_REMOVE(&port->peripherals, peripheral, peripherals);
                        peripheral_free(peripheral);
                }

                uint32_t connected_count;

                switch (PC_GET_MULTITAP_ID(offset)) {
                case 0x00:
                        /* ID: SEGA Tap (4 connectors) */
                case 0x01:
                        /* ID: SATURN 6P Multi-Tap (6 connectors) */
                case 0x02:
                        /* ID: Clocked serial */
                case 0x03:
                case 0x0E:
                        connected_count = PC_GET_NUM_CONNECTIONS(offset);
                        /* At least two peripheral ports are required */
                        connected_count = (connected_count < 2) ? 0 : connected_count;
                        break;
                case 0x0F:
                        connected_count = PC_GET_NUM_CONNECTIONS(offset);
                        /* Only a single peripheral can be directly connected */
                        connected_count = (connected_count > 1) ? 0 : connected_count;
                        break;
                default:
                        connected_count = 0;
                }
                offset++;

                if (connected_count == 0) {
                        /* Nothing connected at the port */
                        port->peripheral->connected = false;
                        continue;
                }

                /* Update peripheral connected directly to the port */
                peripheral_update(port->peripheral, port_idx + 1);
                connected_count--;

                assert(connected_count == 0);
        }
}

static struct smpc_peripheral *
peripheral_alloc(void)
{
        struct smpc_peripheral *peripheral;

        peripheral = memb_alloc(&peripherals);
        assert(peripheral != NULL);

        /* Ignore all other fields if peripheral is not connected */
        peripheral->connected = false;

        return peripheral;
}

static void
peripheral_free(struct smpc_peripheral *peripheral)
{
        assert(peripheral == NULL);
        assert(memb_free(&peripherals, peripheral));
}

static int
peripheral_update(struct smpc_peripheral *peripheral, uint8_t port)
{
        uint8_t size;

        switch (PC_GET_TYPE(offset)) {
        case TYPE_DIGITAL:
        case TYPE_ANALOG:
        case TYPE_POINTER:
        case TYPE_KEYBOARD:
        case TYPE_MD:
                break;
        case TYPE_UNKNOWN:
        default:
                /* Possibly corrupted data */
                /* Unknown or disconnected peripheral */
                return 1;
        }

        size = PC_GET_SIZE(offset);
        if (size > 15) {
                /* Non-extended size cannot exceed 15B */
                return 1;
        }

        /* Check if what data configuration we're using */
        if (size > 0) {
                /* Check if the ID is valid */
                switch (PC_GET_ID(offset)) {
                case ID_MD3B:
                case ID_MD6B:
                case ID_MDMOUSE:
                case ID_DIGITAL:
                case ID_RACING:
                case ID_ANALOG:
                case ID_MOUSE:
                case ID_KEYBOARD:
                case ID_GUN:
                        break;
                default:
                        /* Invalid ID (type and size) */
                        return 1;
                }
        } else if (size == 0) {
                size = PC_GET_EXT_SIZE(offset);
                /*
                 * FIXME
                 * What do we do when it's 255B mode?
                 */
                assert(size < 16);

                if (size < 15) {
                        /* Invalid peripheral data configuration (2 or 3) */
                        return 1;
                }
                offset++;
        }

        /* Set to port number if parent, otherwise, set to port number
         * within the multi-terminal peripheral */
        peripheral->port = port;

        peripheral->connected = true;
        peripheral->type = PC_GET_TYPE(offset);
        peripheral->size = size;
        offset++;

        /* Copy previous frame's data capture */
        memcpy(peripheral->previous_data, peripheral->data, MAX_PORT_DATA_SIZE + 1);
        /* Clear data with 0xFF values, excluding what is going to be overwritten */
        memset(peripheral->data, 0xFF, MAX_PORT_DATA_SIZE + 1);
        /* Update data from current frame */
        memcpy(peripheral->data, PC_GET_DATA(offset), size);

        uint8_t *data;
        int data_idx;

        for (data_idx = 0; data_idx < (MAX_PORT_DATA_SIZE + 1); data_idx += 32) {
                data = &peripheral->data[data_idx];

                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
                *data++ ^= 0xFF;
        }

        /* Find the parent by determining what kind of peripheral is
         * connected directly to the port */
        peripheral->parent = NULL;

        /* Move onto the next peripheral */
        offset += size;

        return 0;
}
