/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>

#include <bios.h>
#include <cpu.h>
#include <scu/ic.h>
#include <smpc/peripheral.h>
#include <smpc/rtc.h>
#include <smpc/smc.h>

#include <mm/memb.h>

#include <sys/queue.h>

#include "smpc-internal.h"

#define OPE     0x02 /* Acquisition Time Optimization */
#define PEN     0x08 /* Peripheral Data Enable */
#define P1MD0   0x10 /* 255-byte mode */
#define P1MD1   0x30 /* 0-byte mode (port is not accessed) */
#define P2MD0   0x40 /* 255-byte mode */
#define P2MD1   0xC0 /* 0-byte mode (port is not accessed) */

/* SR */
#define NPE     0x20 /* Remaining Peripheral Existence */
#define PDE     NPE /* Same as NPE; possibly an error in the documentation? */
#define PDL     0x40 /* Peripheral Data Location */

/* IREG0 for INTBACK */
#define BR      0x40
#define CONT    0x80

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

cpu_smpc_time_t time;

smpc_peripheral_port_t smpc_peripheral_port_1;
smpc_peripheral_port_t smpc_peripheral_port_2;

static volatile bool _collection_complete = false;
static volatile uint32_t _oreg_offset = 0;

/* OREG buffer that can hold a maximum of 6 peripherals with a data size
 * of 255-bytes (+1 for alignment) as well as an entire buffer for SMPC
 * status (time, cartridge code, area code, etc.) */
static uint8_t _oreg_buf[(MAX_PERIPHERALS * (MAX_PERIPHERAL_DATA_SIZE + 1)) + SMPC_OREGS];

static void port_peripherals_free(smpc_peripheral_port_t *);
static smpc_peripheral_t *peripheral_alloc(void);
static void peripheral_free(smpc_peripheral_t *);
static int32_t peripheral_update(smpc_peripheral_port_t *,
    smpc_peripheral_t *, uint8_t);

static void _system_manager_handler(void);

/* A memory pool that holds two peripherals directly connected to each
 * port that also hold MAX_PERIPHERALS (6) each making a total of 14
 * possible peripherals connected at one time */
MEMB(peripherals, smpc_peripheral_t, (2 * MAX_PERIPHERALS) + MAX_PORTS, 4);

void
__smpc_peripheral_init(void)
{
        memb_init(&peripherals);

        smpc_peripheral_port_1.peripheral = peripheral_alloc();
        TAILQ_INIT(&smpc_peripheral_port_1.peripherals);

        smpc_peripheral_port_2.peripheral = peripheral_alloc();
        TAILQ_INIT(&smpc_peripheral_port_2.peripherals);

        /* Set both ports to "SMPC" control mode */
        MEMORY_WRITE(8, SMPC(EXLE1), 0x00);
        MEMORY_WRITE(8, SMPC(IOSEL1), 0x00);
        MEMORY_WRITE(8, SMPC(DDR1), 0x00);
        MEMORY_WRITE(8, SMPC(PDR1), 0x00);

        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_IC_MASK_SYSTEM_MANAGER);
        scu_ic_ihr_set(SCU_IC_INTERRUPT_SYSTEM_MANAGER, &_system_manager_handler);
        scu_ic_mask_chg(~SCU_IC_MASK_SYSTEM_MANAGER, SCU_IC_MASK_NONE);
}

void
smpc_peripheral_intback_issue(void)
{
        /* Send "INTBACK" "SMPC" command */
        /* Set to 255-byte mode for both ports; time optimized
         *
         * Return peripheral data and time, cartridge code, area
         * code, etc.*/
        smpc_smc_intback_call(0x01, P1MD0 | P2MD0 | PEN | OPE);
}

void
smpc_peripheral_process(void)
{
        static smpc_peripheral_port_t *ports[] = {
                &smpc_peripheral_port_1,
                &smpc_peripheral_port_2,
                NULL
        };

        if (!_collection_complete) {
                return;
        }

        _collection_complete = false;

        _oreg_offset = 0;

        /* Ignore OREG0 */
        _oreg_offset++;

        time.year = (OREG_GET(_oreg_offset) << 8) | OREG_GET(_oreg_offset + 1);
        _oreg_offset++;
        _oreg_offset++;

        time.day = OREG_GET(_oreg_offset) & 0xF0;
        time.month = OREG_GET(_oreg_offset) & 0x0F;
        _oreg_offset++;

        time.days = OREG_GET(_oreg_offset);
        _oreg_offset++;

        time.hours = OREG_GET(_oreg_offset);
        _oreg_offset++;

        time.minutes = OREG_GET(_oreg_offset);
        _oreg_offset++;

        time.seconds = OREG_GET(_oreg_offset);
        _oreg_offset++;

        /* Ignore OREG8
         * Ignore OREG9
         * Ignore OREG10
         * Ignore OREG11
         * ... */

        int32_t port_idx;

        /* Peripheral data starts at offset 32 (OREG0) in the OREG buffer */
        _oreg_offset = SMPC_OREGS;

        for (port_idx = 0; ports[port_idx] != NULL; port_idx++) {
                smpc_peripheral_port_t *port;

                port = ports[port_idx];

                int32_t connected;

                /* Update peripheral connected directly to the port */
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
                                smpc_peripheral_t *peripheral;

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
port_peripherals_free(smpc_peripheral_port_t *port)
{
        assert(port != NULL);

        smpc_peripheral_t *peripheral;
        smpc_peripheral_t *tmp_peripheral;

        for (peripheral = TAILQ_FIRST(&port->peripherals);
             (peripheral != NULL) && (tmp_peripheral = TAILQ_NEXT(peripheral, peripherals), 1);
             peripheral = tmp_peripheral) {
                TAILQ_REMOVE(&port->peripherals, peripheral, peripherals);

                peripheral_free(peripheral);
        }
}

static smpc_peripheral_t *
peripheral_alloc(void)
{
        smpc_peripheral_t *peripheral;

        peripheral = memb_alloc(&peripherals);
        assert(peripheral != NULL);

        /* Ignore all other fields if peripheral is not connected */
        peripheral->connected = 0;

        return peripheral;
}

static void
peripheral_free(smpc_peripheral_t *peripheral)
{
        assert(peripheral != NULL);

        int ret __unused;
        ret = memb_free(&peripherals, peripheral);
        assert(ret == 0);
}

static int32_t
peripheral_update(smpc_peripheral_port_t *parent,
    smpc_peripheral_t *peripheral,
    uint8_t port)
{
        uint8_t multitap_id;
        uint32_t connected;
        connected = 0;

        if (parent == NULL) {
                multitap_id = PC_GET_MULTITAP_ID(_oreg_offset);
                switch (multitap_id) {
                case 0x00:
                        /* ID: SEGA Tap (4 connectors) */
                case 0x01:
                        /* ID: SATURN 6P Multi-Tap (6 connectors) */
                case 0x02:
                        /* ID: Clocked serial */
                case 0x03:
                case 0x0E:
                        connected = PC_GET_NUM_CONNECTIONS(_oreg_offset);
                        /* At least two peripheral ports are required */
                        connected = (connected < 2) ? 0 : connected;
                        break;
                case 0x0F:
                        connected = PC_GET_NUM_CONNECTIONS(_oreg_offset);
                        /* Only a single peripheral can be directly connected */
                        connected = (connected > 1) ? 0 : connected;
                        break;
                default:
                        connected = 0;
                }

                _oreg_offset++;
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
        type = PC_GET_TYPE(_oreg_offset);
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

        size = PC_GET_SIZE(_oreg_offset);
        if (size > 0) {
                /* Peripheral data collection #1 */
                /* Check if the ID is valid */
                switch (PC_GET_ID(_oreg_offset)) {
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
                _oreg_offset++;
                size = PC_GET_EXT_SIZE(_oreg_offset);

                /* With a multi-tap, it is possible to mix the
                 * connection of peripherals of 15-bytes or less and
                 * 16-bytes or more. */
        }
        _oreg_offset++;

        switch (size) {
        case 0x02:
                peripheral->previous_data[0] = peripheral->data[0];
                peripheral->previous_data[1] = peripheral->data[1];

                peripheral->data[0] = PC_GET_DATA_BYTE(_oreg_offset, 0) ^ 0xFF;
                peripheral->data[1] = PC_GET_DATA_BYTE(_oreg_offset, 1) ^ 0xFF;
                break;
        case 0x06:
                peripheral->previous_data[0] = peripheral->data[0];
                peripheral->previous_data[1] = peripheral->data[1];
                peripheral->previous_data[2] = peripheral->data[2];
                peripheral->previous_data[3] = peripheral->data[3];
                peripheral->previous_data[4] = peripheral->data[4];
                peripheral->previous_data[5] = peripheral->data[5];

                peripheral->data[0] = PC_GET_DATA_BYTE(_oreg_offset, 0) ^ 0xFF;
                peripheral->data[1] = PC_GET_DATA_BYTE(_oreg_offset, 1) ^ 0xFF;
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
        _oreg_offset += size;

        return connected;
}

static void
_system_manager_handler(void)
{
        static uint32_t offset = 0;

        /* We don't have much time in the critical section. Just
         * buffer the registers */
        uint32_t oreg;

        for (oreg = 0; oreg < SMPC_OREGS; oreg++, offset++) {
                OREG_SET(offset, MEMORY_READ(8, OREG(oreg)));
        }

        uint8_t sr;
        sr = MEMORY_READ(8, SMPC(SR));

        if ((sr & 0x80) == 0x80) {
                if ((sr & NPE) == 0x00) {
                        /* Mark that SMPC status and peripheral data
                         * collection is complete */
                        _collection_complete = true;

                        offset = 0;

                        /* Issue a "BREAK" for the "INTBACK" command */
                        MEMORY_WRITE(8, IREG(0), BR);
                        return;
                }
        }

        /* Issue a "CONTINUE" for the "INTBACK" command */
        MEMORY_WRITE(8, IREG(0), CONT);
}
