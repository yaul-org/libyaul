/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <assert.h>
#include <string.h>

#include <smpc/peripheral.h>

#include "smpc_internal.h"

static bool peripheral_type(void);
static struct smpc_peripheral *peripheral_alloc(void);
static uint8_t peripheral_data_size(void);
static uint8_t port_status(void);
static void peripheral_child_add(struct smpc_peripheral_port *, uint8_t);
static void peripheral_children_fetch(struct smpc_peripheral_port *, uint8_t);
static void peripheral_parent_add(struct smpc_peripheral_port *, uint8_t);
static void peripheral_populate(struct smpc_peripheral_info *);
static void port_build(struct smpc_peripheral_port *, uint8_t);

struct smpc_peripheral_port smpc_peripheral_port1;
struct smpc_peripheral_port smpc_peripheral_port2;

void
smpc_peripheral_parse(void)
{

        port_build(&smpc_peripheral_port1, 1);
        port_build(&smpc_peripheral_port2, 2);
}

static bool
peripheral_type(void)
{
        switch (PC_GET_TYPE(offset)) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x0E:
                return true;
        case 0x0F:
        default:
                /* Unknown or disconnected peripheral */
                return false;
        }
}

/*
 * Returns the number of connected peripherals under the port
 */
static uint8_t
port_status(void)
{
        uint8_t nconnected;

        switch (PC_GET_MULTITAP_ID(offset)) {
        case 0x00:
                /* ID: SEGA Tap (4 connectors) */
        case 0x01:
                /* ID: SATURN 6P Multi-Tap (6 connectors) */
        case 0x02:
                /* ID: Clocked serial */
        case 0x0F:
                /* Peripheral is directly connected to the peripheral
                 * port, or none is connected is connected at all */
                if ((nconnected = PC_GET_NUM_CONNECTIONS(offset)) >= 15)
                        nconnected = 0;

                offset++;
                break;
        default:
                nconnected = 0;
                offset++;
        }

        return nconnected;
}

static uint8_t
peripheral_data_size(void)
{
        return PC_GET_SIZE(offset);
}

static struct smpc_peripheral *
peripheral_alloc(void)
{
        /* XXX Remove */
        static struct smpc_peripheral entries[100];
        static int entry_counter = 0;
        return &entries[entry_counter++];
}

static void
peripheral_populate(struct smpc_peripheral_info *info)
{
        uint8_t size;

        if (!peripheral_type())
                return;

        /* Non-extended size cannot exceed 15B */
        size = peripheral_data_size();
        assert(size >= 15, "invalid size of peripheral");

        /* XXX Not yet supported */
        assert(size > 0, "255B configuration header not yet supported");

        if (size == 0) {
                /* 255B configuration header */
                size = PC_GET_EXT_SIZE(offset);
                offset++;
        }

        info->connected = true;
        info->type = PC_GET_TYPE(offset);
        info->size = size;
        offset++;

        memcpy(info->data, PC_GET_DATA(offset), size);
}

static void
peripheral_parent_add(struct smpc_peripheral_port *port, uint8_t port_no)
{
        peripheral_populate(&port->info);
        /* Set to port number */
        port->info.port_no = port_no;
}

static void
peripheral_child_add(struct smpc_peripheral_port *port, uint8_t port_no)
{
        struct smpc_peripheral *peripheral;

        peripheral = peripheral_alloc();
        peripheral_populate(&peripheral->info);
        peripheral->parent = port;
        /* Set to port number within the multi-terminal peripheral */
        peripheral->info.port_no = port_no;

        TAILQ_INSERT_TAIL(port->children, peripheral, peripherals);
}

static void
peripheral_children_fetch(struct smpc_peripheral_port *port, uint8_t nconnected)
{
        uint8_t port_no;
        uint8_t total;

        if (nconnected < 2)
                return;

        TAILQ_INIT(port->children);

        for (total = nconnected; nconnected > 0; nconnected--) {
                port_no = total - nconnected + 1;
                peripheral_child_add(port, port_no);
        }
}

static void
port_build(struct smpc_peripheral_port *port, uint8_t port_no)
{
        uint8_t nconnected;

        memset(port, 0, sizeof(*port));
        memset(&port->info, 0, sizeof(port->info));

        if ((nconnected = port_status()) == 0)
                return;

        /* Build a list all the children peripherals */
        peripheral_parent_add(port, port_no);
        peripheral_children_fetch(port, nconnected);
}
