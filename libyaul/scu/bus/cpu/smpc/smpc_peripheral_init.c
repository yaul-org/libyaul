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

#define P1MD0   0x00 /* 15B mode */
#define P1MD1   0x02 /* 255B mode */
#define P1MD2   0x03 /* 0B mode (port is not accessed) */
#define P2MD0   0x00
#define P2MD1   0x04
#define P2MD2   0x0C
#define RESB    0x10
#define NPE     0x20
#define PDL     0x40

struct smpc_peripheral_port smpc_peripheral_port_1;
struct smpc_peripheral_port smpc_peripheral_port_2;

uint8_t oreg_buf[MAX_PORT_DEVICES * (MAX_PORT_DATA_SIZE + 1)];

static int offset = 0;

static void port_peripherals_free(struct smpc_peripheral_port *);
static struct smpc_peripheral *peripheral_alloc(void);
static void peripheral_free(struct smpc_peripheral *);
static int peripheral_update(struct smpc_peripheral *, uint8_t);

static void handler_system_manager(void);

static void irq_mux_vblank_in(irq_mux_handle_t *irq_mux __attribute__ ((unused)));
static void irq_mux_hblank_in(irq_mux_handle_t *irq_mux __attribute__ ((unused)));

MEMB(peripherals, struct smpc_peripheral, 2, 2);

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

        if (!(TAILQ_EMPTY(&port->peripherals))) {
                struct smpc_peripheral *peripheral;
                struct smpc_peripheral *tmp_peripheral;

                for (peripheral = TAILQ_FIRST(&port->peripherals);
                     peripheral != NULL;
                     peripheral = tmp_peripheral) {
                        tmp_peripheral = TAILQ_NEXT(peripheral, peripherals);
                        TAILQ_REMOVE(&port->peripherals, peripheral, peripherals);
                        peripheral_free(peripheral);
                }
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
        /* Clear data */
        memset(peripheral->data, 0x00, MAX_PORT_DATA_SIZE + 1);
        memset(peripheral->previous_data, 0x00, MAX_PORT_DATA_SIZE + 1);

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
                return -1;
        }

        size = PC_GET_SIZE(offset);
        if (size > 15) {
                /* Non-extended size cannot exceed 15B */
                return -1;
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
                        return -1;
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
                        return -1;
                }
                offset++;
        }

        peripheral->connected = true;

        /* Set to port number if parent, otherwise, set to port number
         * within the multi-terminal peripheral */
        peripheral->port = port;
        peripheral->type = PC_GET_TYPE(offset);
        peripheral->size = size;
        offset++;

        switch (size) {
        case 0x02:
                peripheral->previous_data[0] = peripheral->data[0];
                peripheral->previous_data[1] = peripheral->data[1];

                peripheral->data[0] = PC_GET_DATA_BYTE(offset, 0) ^ 0xFF;
                peripheral->data[1] = PC_GET_DATA_BYTE(offset, 1) ^ 0xFF;
                break;
        default:
                /* XXX
                 * Currently not supporting other sizes */
                assert(false);
        }
        /* Move onto the next peripheral */
        offset += size;

        /* Find the parent by determining what kind of peripheral is
         * connected directly to the port */
        peripheral->parent = NULL;

        return 0;
}

static void
handler_system_manager(void)
{
        /* Fetch but no parsing */
        static uint16_t oreg_buf_idx = 0;

        /* Is this the first time fetching peripheral data? */
        if (((MEMORY_READ(8, SMPC(SR))) & PDL) == PDL) {
                oreg_buf_idx = 0;
        }

        /* What if we exceed our capacity? Buffer overflow */
        assert(oreg_buf_idx <= MAX_PORT_DATA_SIZE);

        uint32_t ooffset;

        for (ooffset = 0; ooffset < SMPC_OREGS; ooffset++, oreg_buf_idx++) {
                /* We don't have much time in the critical section. Just
                 * buffer the registers */
                OREG_SET(oreg_buf_idx, MEMORY_READ(8, OREG(ooffset)));
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

        offset = 0;
        for (port_idx = 0; ports[port_idx] != NULL; port_idx++) {
                port = ports[port_idx];

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
                        break;
                }
                offset++;

                if (connected_count == 0) {
                        /* Nothing connected at the port */
                        port->peripheral->connected = false;
                        port_peripherals_free(port);
                        continue;
                }

                /* Update peripheral connected directly to the port */
                if ((peripheral_update(port->peripheral, port_idx + 1)) < 0) {
                        /* Couldn't parse data; invalid peripheral */
                        port->peripheral->connected = false;
                        port_peripherals_free(port);
                        continue;
                }
                connected_count--;

                /* For now, assume that only standard digital pads can be connected */
                assert(connected_count == 0);
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

        /*
         * Send "INTBACK" "SMPC" command (300us after VBLANK-IN)
         * Fetch both
         * Set to 15-byte mode; optimized
         */

        if ((vdp2_tvmd_vcount_get()) == (224 + 8)) {
                smpc_smc_intback_call(0x00, 0x0A);
        }
}
