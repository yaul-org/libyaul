/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <smpc/peripheral.h>

#include "smpc_internal.h"

#define	ID_UNCONNECTED  0xFF
#define	ID_MD3B         0xE1
#define	ID_MD6B         0xE2
#define	ID_MDMOUSE      0xE3
#define	ID_DIGITAL      0x02
#define	ID_RACING       0x13
#define	ID_ANALOG       0x15
#define	ID_MOUSE        0x23
#define	ID_KEYBOARD     0x34
#define	ID_GUN          0xFA

#define	TYPE_DIGITAL    0x00
#define	TYPE_ANALOG     0x10
#define	TYPE_POINTER    0x20
#define	TYPE_KEYBOARD   0x30
#define	TYPE_MD         0xE0
#define	TYPE_UNKNOWN	0xF0

static bool peripheral_type(void);
static struct smpc_peripheral *peripheral_alloc(void);
static uint8_t peripheral_data_size(void);
static uint8_t port_status(void);
static void peripheral_child_add(struct smpc_peripheral_port *, uint8_t);
static void peripheral_children_fetch(struct smpc_peripheral_port *, uint8_t);
static void peripheral_parent_add(struct smpc_peripheral_port *, uint8_t);
static void peripheral_populate(struct smpc_peripheral_info *);
static void port_build(struct smpc_peripheral_port *, uint8_t);
static char *dump_registers(void);

uint8_t offset = 0;

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
        /* XXX Remove and replace with TLSF */
        static struct smpc_peripheral peripheral[32];
        static int count = 0;

        return &peripheral[count++];
}

static void
peripheral_populate(struct smpc_peripheral_info *info)
{
        uint8_t *data;
        uint8_t size;

        /* Type cannot be invalid at this point */
        assert((peripheral_type()), "invalid peripheral type");

        /* Non-extended size cannot exceed 15B */
        size = peripheral_data_size();
        assert(size <= 15, "invalid size of peripheral");

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

        data = &info->data[size];
        memset(data, 0xFF, MAX_PORT_DATA_SIZE - size);
        memcpy(info->data, PC_GET_DATA(offset), size);
        offset += size;
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
                /* No children */
                return;

        /* XXX Initialize queue of children */
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

static char * __attribute__ ((used))
dump_registers(void)
{
        static char buf[2048];

        /* XXX There's a nasty bug where half of the screen is corrupted */
        (void)sprintf(buf, "\n\n"
            " OREG[0] = 0x%02X OREG[16] = 0x%02X\n"
            " OREG[1] = 0x%02X OREG[17] = 0x%02X\n"
            " OREG[2] = 0x%02X OREG[18] = 0x%02X\n"
            " OREG[3] = 0x%02X OREG[19] = 0x%02X\n"
            " OREG[4] = 0x%02X OREG[20] = 0x%02X\n"
            " OREG[5] = 0x%02X OREG[21] = 0x%02X\n"
            " OREG[6] = 0x%02X OREG[22] = 0x%02X\n"
            " OREG[7] = 0x%02X OREG[23] = 0x%02X\n"
            " OREG[8] = 0x%02X OREG[24] = 0x%02X\n"
            " OREG[9] = 0x%02X OREG[25] = 0x%02X\n"
            "OREG[10] = 0x%02X OREG[26] = 0x%02X\n"
            "OREG[11] = 0x%02X OREG[27] = 0x%02X\n"
            "OREG[12] = 0x%02X OREG[28] = 0x%02X\n"
            "OREG[13] = 0x%02X OREG[29] = 0x%02X\n"
            "OREG[14] = 0x%02X OREG[30] = 0x%02X\n"
            "OREG[15] = 0x%02X OREG[31] = 0x%02X\n",
            OREG_GET(0), OREG_GET(16),
            OREG_GET(1), OREG_GET(17),
            OREG_GET(2), OREG_GET(18),
            OREG_GET(3), OREG_GET(19),
            OREG_GET(4), OREG_GET(20),
            OREG_GET(5), OREG_GET(21),
            OREG_GET(6), OREG_GET(22),
            OREG_GET(7), OREG_GET(23),
            OREG_GET(8), OREG_GET(24),
            OREG_GET(9), OREG_GET(25),
            OREG_GET(10), OREG_GET(26),
            OREG_GET(11), OREG_GET(27),
            OREG_GET(12), OREG_GET(28),
            OREG_GET(13), OREG_GET(29),
            OREG_GET(14), OREG_GET(30),
            OREG_GET(15), OREG_GET(31));

        return buf;
}
