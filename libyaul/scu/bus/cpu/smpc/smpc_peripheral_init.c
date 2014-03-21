/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu.h>
#include <irq-mux.h>
#include <scu/ic.h>
#include <smpc/peripheral.h>
#include <smpc/smc.h>
#include <vdp2.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lib/memb.h>

#include <sys/queue.h>

#include "smpc-internal.h"

#define OPE     0x02 /* Acquisition Time Optimization */
#define PEN     0x08 /* Peripheral Data Enable */
#define P1MD0   0x10 /* 15-byte mode */
#define P1MD1   0x20 /* 255-byte mode */
#define P1MD2   (P1MD0 | P1MD1) /* Port #1 0B mode (port is not accessed) */
#define P2MD0   0x40 /* 15-byte mode */
#define P2MD1   0x80 /* 255-byte mode */
#define P2MD2   (P2MD0 | P2MD1) /* Port #2 0B mode (port is not accessed) */

/* SR */
#define NPE     0x20 /* Remaining Peripheral Existence */
#define PDL     0x40 /* Peripheral Data Location */

#define OREG_SET(x, y) do {                                                    \
        _oreg_buf[(x)] = (y);                                                  \
} while(0)

#define OREG_GET(x)     (_oreg_buf[(x)])
#define OREG_OFFSET(x)  (&_oreg_buf[(x)])

/* 1st data byte */
#define PC_GET_MULTITAP_ID(x)   ((OREG_GET((x)) >> 4) & 0x0F)
#define PC_GET_NUM_CONNECTIONS(x) (OREG_GET((x)) & 0x0F)
/* 2nd data byte */
#define PC_GET_ID(x)            (OREG_GET((x)) & 0xFF)
#define PC_GET_TYPE(x)          ((OREG_GET((x)) >> 4) & 0x0F)
#define PC_GET_SIZE(x)          (OREG_GET((x)) & 0x0F)
/* 3rd data byte */
#define PC_GET_EXT_SIZE(x)      (OREG_GET((x)) & 0xFF)
/* 3rd (or 4th) data byte */
#define PC_GET_DATA(x)          (OREG_OFFSET((x)))
#define PC_GET_DATA_BYTE(x, y)  (((uint8_t *)OREG_OFFSET((x)))[(y)])

struct smpc_peripheral_port smpc_peripheral_port_1;
struct smpc_peripheral_port smpc_peripheral_port_2;

static uint32_t _offset = 0;
static uint8_t _oreg_buf[MAX_PORT_DEVICES * (MAX_PORT_DATA_SIZE + 1)];

static void port_peripherals_free(struct smpc_peripheral_port *);
static struct smpc_peripheral *peripheral_alloc(void);
static void peripheral_free(struct smpc_peripheral *);
static int32_t peripheral_update(struct smpc_peripheral_port *,
    struct smpc_peripheral *, uint8_t);

static void handler_system_manager(void);

static void irq_mux_vblank_in(irq_mux_handle_t * __attribute__ ((unused)));
static void irq_mux_hblank_in(irq_mux_handle_t * __attribute__ ((unused)));

MEMB(peripherals, struct smpc_peripheral, MAX_PORT_DEVICES + 2, 4);

void
smpc_peripheral_init(void)
{
        memb_init(&peripherals);

        smpc_peripheral_port_1.peripheral = peripheral_alloc();
        TAILQ_INIT(&smpc_peripheral_port_1.peripherals);

        smpc_peripheral_port_2.peripheral = peripheral_alloc();
        TAILQ_INIT(&smpc_peripheral_port_2.peripherals);

        /* Disablo interrupts */
        cpu_intc_disable();

        /* Set both ports to "SMPC" control mode */
        MEMORY_WRITE(8, SMPC(EXLE1), 0x00);
        MEMORY_WRITE(8, SMPC(IOSEL1), 0x00);
        MEMORY_WRITE(8, SMPC(DDR1), 0x00);
        MEMORY_WRITE(8, SMPC(PDR1), 0x00);

        /* Acquisition at start (+ 5 scanlines) of HBLANK-IN */
        irq_mux_t *hblank_in;
        hblank_in = vdp2_tvmd_hblank_in_irq_get();
        irq_mux_handle_add(hblank_in, irq_mux_hblank_in, NULL);

        /* Parse at start of VBLANK-IN */
        irq_mux_t *vblank_in;

        vblank_in = vdp2_tvmd_vblank_in_irq_get();
        irq_mux_handle_add(vblank_in, irq_mux_vblank_in, NULL);

        uint32_t mask;

        mask = IC_MASK_SYSTEM_MANAGER;
        scu_ic_mask_chg(IC_MASK_ALL, mask);

        scu_ic_interrupt_set(IC_INTERRUPT_SYSTEM_MANAGER,
            &handler_system_manager);
        scu_ic_mask_chg(IC_MASK_ALL & ~mask, IC_MASK_NONE);

        /* Enable interrupts */
        cpu_intc_enable();
}

static void
port_peripherals_free(struct smpc_peripheral_port *port)
{

        assert(port != NULL);

        struct smpc_peripheral *peripheral;
        struct smpc_peripheral *tmp_peripheral;

        for (peripheral = TAILQ_FIRST(&port->peripherals);
             (peripheral != NULL) && (tmp_peripheral = TAILQ_NEXT(peripheral, peripherals), 1);
             peripheral = tmp_peripheral) {
                TAILQ_REMOVE(&port->peripherals, peripheral, peripherals);

                peripheral_free(peripheral);
        }
}

static struct smpc_peripheral *
peripheral_alloc(void)
{
        struct smpc_peripheral *peripheral;

        peripheral = memb_alloc(&peripherals);
        assert(peripheral != NULL);

        /* Ignore all other fields if peripheral is not connected */
        peripheral->connected = 0;

        return peripheral;
}

static void
peripheral_free(struct smpc_peripheral *peripheral)
{
        assert(peripheral != NULL);
        assert((memb_free(&peripherals, peripheral)) == 0);
}

static int32_t
peripheral_update(struct smpc_peripheral_port *parent,
    struct smpc_peripheral *peripheral,
    uint8_t port)
{
        uint8_t multitap_id;
        uint32_t connected;

        if (parent == NULL) {
                multitap_id = PC_GET_MULTITAP_ID(_offset);
                switch (multitap_id) {
                case 0x00:
                        /* ID: SEGA Tap (4 connectors) */
                case 0x01:
                        /* ID: SATURN 6P Multi-Tap (6 connectors) */
                case 0x02:
                        /* ID: Clocked serial */
                case 0x03:
                case 0x0E:
                        connected = PC_GET_NUM_CONNECTIONS(_offset);
                        /* At least two peripheral ports are required */
                        connected = (connected < 2) ? 0 : connected;
                        break;
                case 0x0F:
                        connected = PC_GET_NUM_CONNECTIONS(_offset);
                        /* Only a single peripheral can be directly connected */
                        connected = (connected > 1) ? 0 : connected;
                        break;
                default:
                        connected = 0;
                }

                _offset++;
        }

        if (connected == 0) {
                /* Nothing directly connected to the port */
                return -1;
        }

        uint8_t type;
        uint8_t size;

        if (connected > 1) {
                peripheral->connected = connected;
                peripheral->port = port;
                peripheral->type = multitap_id;
                peripheral->size = 0x00;
                peripheral->parent = parent;

                return connected;
        }

        /* Check if the type is valid */
        type = PC_GET_TYPE(_offset);
        switch (type) {
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
                return -1;
        }

        size = PC_GET_SIZE(_offset);
        if (size > 0) {
                /* Peripheral data collection #1 */
                /* Check if the ID is valid */
                switch (PC_GET_ID(_offset)) {
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
                        return -1;
                }
        } else if (size == 0) {
                /* Peripheral data collection #2 is not supported as
                 * we're always in 255-byte mode.
                 *
                 * Peripheral data collection #3 */
                _offset++;
                size = PC_GET_EXT_SIZE(_offset);

                /* With a multi-tap, it is possible to mix the
                 * connection of peripherals of 15-bytes or less and
                 * 16-bytes or more. */
        }
        _offset++;

        uint32_t data_idx;

        switch (size) {
        case 0x02:
                peripheral->previous_data[0] = peripheral->data[0];
                peripheral->previous_data[1] = peripheral->data[1];

                peripheral->data[0] = PC_GET_DATA_BYTE(_offset, 0) ^ 0xFF;
                peripheral->data[1] = PC_GET_DATA_BYTE(_offset, 1) ^ 0xFF;
                break;
        case 0x06:
                memset(&peripheral->previous_data[0], peripheral->data[0], size);

                peripheral->data[0] = PC_GET_DATA_BYTE(_offset, 0) ^ 0xFF;
                peripheral->data[1] = PC_GET_DATA_BYTE(_offset, 1) ^ 0xFF;
                break;
        default:
                /* XXX
                 * Currently not supporting other sizes */
                assert(false);
        }

        peripheral->connected = 1;
        peripheral->port = port;
        peripheral->type = multitap_id;
        peripheral->size = 0x00;
        peripheral->parent = parent;

        /* Move onto the next peripheral */
        _offset += size;

        return connected;
}

static void
handler_system_manager(void)
{
        /* Fetch but no parsing */
        static uint32_t offset = 0;

        /* Is this the first time fetching peripheral data? */
        if (((MEMORY_READ(8, SMPC(SR))) & PDL) == PDL) {
                offset = 0;
        }

        /* What if we exceed our capacity? Buffer overflow */
        assert(offset <= MAX_PORT_DATA_SIZE);

        uint32_t oreg_idx;

        for (oreg_idx = 0; oreg_idx < SMPC_OREGS; oreg_idx++, offset++) {
                /* We don't have much time in the critical section. Just
                 * buffer the registers */
                OREG_SET(offset, MEMORY_READ(8, OREG(oreg_idx)));
        }

        /* Check if there is any remaining peripheral data */
        if (((MEMORY_READ(8, SMPC(SR))) & NPE) == 0x00) {
                /* Issue a "BREAK" for the "INTBACK" command */
                MEMORY_WRITE(8, IREG(0), 0x40);
                return;
        }

        /* Is there remaining peripheral data? */
        /* Let's not yet cover this case yet since we can't fetch more data */
        assert(((MEMORY_READ(8, SMPC(SR))) & NPE) == 0x00);

        /* Issue a "CONTINUE" for the "INTBACK" command */
        MEMORY_WRITE(8, IREG(0), 0x80);
}

static void
irq_mux_vblank_in(irq_mux_handle_t *irq_mux __attribute__ ((unused)))
{
        static struct smpc_peripheral_port *ports[] = {
                &smpc_peripheral_port_1,
                &smpc_peripheral_port_2,
                NULL
        };

        /* Could this help? */
        if (((scu_ic_status_get()) & IC_IST_SYSTEM_MANAGER) == IC_IST_SYSTEM_MANAGER) {
                return;
        }

        int32_t port_idx;

        _offset = 0;
        for (port_idx = 0; ports[port_idx] != NULL; port_idx++) {
                struct smpc_peripheral_port *port;

                port = ports[port_idx];

                int32_t previous_connected; /* Previous connected peripherals */
                int32_t connected;

                /* Update peripheral connected directly to the port */
                previous_connected = port->peripheral->connected;
                if ((connected = peripheral_update(/* parent = */ NULL,
                            port->peripheral, port_idx + 1)) < 0) {
                        /* Couldn't parse data; invalid peripheral */
                        port->peripheral->connected = 0;
                        port_peripherals_free(port);
                        continue;
                }

                port_peripherals_free(port);

                if (connected > 1) {
                        int32_t sub_port;

                        port->peripheral->connected = 0;
                        for (sub_port = 1; connected > 0; connected--, sub_port++) {
                                struct smpc_peripheral *peripheral;

                                peripheral = peripheral_alloc();
                                assert(peripheral != NULL);

                                if ((peripheral_update(/* parent = */ port, peripheral, sub_port)) < 0) {
                                        peripheral_free(peripheral);
                                        continue;
                                }

                                /* Add peripheral */
                                TAILQ_INSERT_TAIL(&port->peripherals, peripheral, peripherals);

                                port->peripheral->connected++;
                        }
                }
        }
}

static void
irq_mux_hblank_in(irq_mux_handle_t *irq_mux __attribute__ ((unused)))
{
        /*
         * From Charles MacDonald's hardware notes:
         *
         * NTSC (224 scanlines)
         *  224 scanlines for the active display area
         *
         *    8 scanlines for the top border area
         *   15 scanlines for the top blanking area
         *
         *    8 scanlines for the bottom border area
         *    5 scanlines for the bottom blanking area
         *
         *    3 scanlines for the vertical sync area
         */

        /* Make the assumption that we're on a NTSC console and it's set
         * at a default 224 scanlines */

        /* Send "INTBACK" "SMPC" command (300us after VBLANK-IN) */
        if ((vdp2_tvmd_vcount_get()) == (224 + 8)) {
                /* Set to 255-byte mode for both ports; not time
                 * optimized.
                 *
                 * Return peripheral data and time, cartridge code, area
                 * code, etc.*/
                smpc_smc_intback_call(0x00, P1MD1 | P2MD1 | PEN);
        }
}
