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

#include "smpc-internal.h"

#define	ID_DIGITAL      0x02
#define	ID_RACING       0x13
#define	ID_ANALOG       0x16
#define	ID_MOUSE        0x23
#define	ID_KEYBOARD     0x34
#define	ID_MD3B         0xE1
#define	ID_MD6B         0xE2
#define	ID_MDMOUSE      0xE3
#define	ID_GUN          0xFA
#define	ID_UNCONNECTED  0xFF

#define	TYPE_DIGITAL    0x00
#define	TYPE_ANALOG     0x01
#define	TYPE_POINTER    0x02
#define	TYPE_KEYBOARD   0x03
#define	TYPE_MD         0x0E
#define	TYPE_UNKNOWN	0x0F

static bool peripheral_id(void);
static bool peripheral_type(void);
static int peripheral_populate_info(struct smpc_peripheral_info *, uint8_t);
static struct smpc_peripheral *peripheral_alloc(void);
static uint8_t peripheral_data_size(void);
static uint8_t port_status(void);
static void peripheral_child_add(struct smpc_peripheral_port *, uint8_t);
static void peripheral_children_fetch(struct smpc_peripheral_port *, uint8_t);
static void peripheral_free(struct smpc_peripheral *);
static void peripheral_parent_add(struct smpc_peripheral_port *, uint8_t);
static void port_build(struct smpc_peripheral_port *, uint8_t);
static void port_reset(struct smpc_peripheral_port *);

#ifdef DEBUG
static char *dump_registers(void);
#endif /* DEBUG */

uint8_t offset = 0;

struct smpc_peripheral_port smpc_peripheral_port1;
struct smpc_peripheral_port smpc_peripheral_port2;

void
smpc_peripheral_parse(irq_mux_handle_t *hdl __attribute__ ((unused)))
{
        uint32_t ist;

        /* Could this help? */
        ist = scu_ic_status_get();
        if ((ist & IC_IST_SYSTEM_MANAGER) == IC_IST_SYSTEM_MANAGER)
                return;

        port_build(&smpc_peripheral_port1, 1);
        port_build(&smpc_peripheral_port2, 2);
}

static bool
peripheral_id(void)
{

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
                return true;
        default:
                return false;
        }
}

static bool
peripheral_type(void)
{
        switch (PC_GET_TYPE(offset)) {
        case TYPE_DIGITAL:
        case TYPE_ANALOG:
        case TYPE_POINTER:
        case TYPE_KEYBOARD:
        case TYPE_MD:
                return true;
        case TYPE_UNKNOWN:
        default:
                /* Unknown or disconnected peripheral */
                return false;
        }
}

static uint8_t
peripheral_data_size(void)
{
        return PC_GET_SIZE(offset);
}

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
        case 0x03:
        case 0x0E:
                nconnected = PC_GET_NUM_CONNECTIONS(offset);
                /* At least two peripheral ports are required */
                nconnected = (nconnected < 2) ? 0 : nconnected;
                break;
        case 0x0F:
                nconnected = PC_GET_NUM_CONNECTIONS(offset);
                /* Only a single peripheral can be directly connected */
                nconnected = (nconnected > 1) ? 0 : nconnected;
                break;
        default:
                nconnected = 0;
        }

        offset++;
        return nconnected;
}

static struct smpc_peripheral *
peripheral_alloc(void)
{
        struct smpc_peripheral *peripheral;

        peripheral = (struct smpc_peripheral *)malloc(sizeof(struct smpc_peripheral));
        if (peripheral == NULL)
                return NULL;

        return peripheral;
}

static void
peripheral_free(struct smpc_peripheral *peripheral)
{
        peripheral = peripheral;
}

static int
peripheral_populate_info(struct smpc_peripheral_info *info, uint8_t port_no)
{
        uint8_t *data;
        uint8_t size;

        if (!(peripheral_type()))
                /* Possibly corrupted data */
                return 1;

        if ((size = peripheral_data_size()) > 15)
                /* Non-extended size cannot exceed 15B */
                return 1;

        /* Check if what data configuration we're using */
        if (size > 0) {
                /* data Configuration 1 */
                if (!(peripheral_id()))
                        /* Invalid ID (type and size) */
                        return 1;
        } else if (size == 0) {
                size = PC_GET_EXT_SIZE(offset);
                /*
                 * FIXME
                 * What do we do when it's 255B mode?
                 */
                assert(size < 16);

                if (size < 15)
                        /* Invalid peripheral data configuration (2 or 3) */
                        return 1;
                offset++;
        }

        /* Set to port number if parent, otherwise, set to port number
         * within the multi-terminal peripheral */
        info->port_no = port_no;

        info->connected = true;
        info->type = PC_GET_TYPE(offset);
        info->size = size;
        offset++;

        data = &info->data[size];
        memset(data, 0xFF, MAX_PORT_DATA_SIZE - size);
        memcpy(info->data, PC_GET_DATA(offset), size);
        offset += size;

        return 0;
}

static void
peripheral_parent_add(struct smpc_peripheral_port *port, uint8_t port_no)
{

        if ((peripheral_populate_info(&port->info, port_no)))
                port_reset(port);
}

static void
peripheral_child_add(struct smpc_peripheral_port *port, uint8_t port_no)
{
        struct smpc_peripheral *peripheral;

        peripheral = peripheral_alloc();
        if ((peripheral_populate_info(&peripheral->info, port_no))) {
                port_reset(port);
                peripheral_free(peripheral);
                return;
        }
        peripheral->parent = port;

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

        /* FIXME
         * Initialize queue of children
         */
        TAILQ_INIT(port->children);

        for (total = nconnected; nconnected > 0; nconnected--) {
                port_no = total - nconnected + 1;
                peripheral_child_add(port, port_no);
        }
}

static void
port_reset(struct smpc_peripheral_port *port)
{

        memset(port, 0, sizeof(*port));
        memset(&port->info, 0, sizeof(port->info));
}

static void
port_build(struct smpc_peripheral_port *port, uint8_t port_no)
{
        uint8_t nconnected;

        port_reset(port);

        if ((nconnected = port_status()) == 0)
                return;

        /* Build a list all the children peripherals */
        peripheral_parent_add(port, port_no);
        peripheral_children_fetch(port, nconnected);
}

#ifdef DEBUG
static char *
dump_registers(void)
{
        static char buf[2048];

        (void)sprintf(buf, "\n\nDump of SMPC Registers:\n"
            "offset = %d\n"
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
            offset,
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
#endif /* DEBUG */
