/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>

#include <sys/queue.h>

#include <bios.h>

#include <scu/ic.h>

#include <smpc/peripheral.h>
#include <smpc/rtc.h>
#include <smpc/smc.h>

#include <mm/memb.h>

#include "smpc-internal.h"

#define OPE     0x02 /* Acquisition Time Optimization */
#define PEN     0x08 /* Peripheral Data Enable */
#define P1MD0   0x10 /* 255-byte mode */
#define P1MD1   0x30 /* 0-byte mode (port is not accessed) */
#define P2MD0   0x40 /* 255-byte mode */
#define P2MD1   0xC0 /* 0-byte mode (port is not accessed) */

/* SR */
#define NPE     0x20 /* Remaining Peripheral Existence */
#define PDE     NPE  /* Same as NPE; possibly an error in the documentation? */
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

smpc_time_t __smpc_time;

smpc_peripheral_port_t __smpc_peripheral_ports[2];

static volatile bool _collection_complete = false;
static volatile uint32_t _oreg_offset = 0;

/* OREG buffer that can hold a maximum of 6 peripherals with a data size
 * of 255-bytes (+1 for alignment) as well as an entire buffer for SMPC
 * status (time, cartridge code, area code, etc.) */
static uint8_t _oreg_buf[(MAX_PERIPHERALS * (MAX_PERIPHERAL_DATA_SIZE + 1)) + SMPC_OREGS];

static void _port_peripherals_free(smpc_peripheral_port_t *per_port);
static smpc_peripheral_t *_peripheral_alloc(void);
static void _peripheral_free(smpc_peripheral_t *peripheral);
static int32_t _peripheral_update(smpc_peripheral_port_t *per_port_parent,
    smpc_peripheral_t *peripheral, uint8_t port);

static void _system_manager_handler(void);

/* A memory pool that holds two peripherals directly connected to each port that
 * also hold MAX_PERIPHERALS (6) each making a total of 14 possible peripherals
 * connected at one time */
MEMB(_peripherals_memb, smpc_peripheral_t, (2 * MAX_PERIPHERALS) + MAX_PORTS, 4);

void
smpc_peripheral_init(void)
{
        memb_init(&_peripherals_memb);

        __smpc_peripheral_ports[0].peripheral = _peripheral_alloc();
        TAILQ_INIT(&__smpc_peripheral_ports[0].peripherals);

        __smpc_peripheral_ports[1].peripheral = _peripheral_alloc();
        TAILQ_INIT(&__smpc_peripheral_ports[1].peripherals);

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
         * Return peripheral data and time, cartridge code, area code, etc */
        smpc_smc_intback_call(0x01, P1MD0 | P2MD0 | PEN | OPE);
}

void
smpc_peripheral_process(void)
{
        if (!_collection_complete) {
                return;
        }

        _collection_complete = false;

        /* Ignore OREG0 */

        __smpc_time.year = (OREG_GET(1) << 8) | OREG_GET(2);
        __smpc_time.week_day = OREG_GET(3) >> 4;
        __smpc_time.month = OREG_GET(3) & 0x0F;
        __smpc_time.day = OREG_GET(4);
        __smpc_time.hours = OREG_GET(5);
        __smpc_time.minutes = OREG_GET(6);
        __smpc_time.seconds = OREG_GET(7);

        /* Ignore OREG8
         * Ignore OREG9
         * Ignore OREG10
         * Ignore OREG11
         * ... */

        /* Peripheral data starts at offset 32 (OREG0) in the OREG buffer */
        _oreg_offset = SMPC_OREGS;

        for (uint32_t port_idx = 0; port_idx < MAX_PORTS; port_idx++) {
                smpc_peripheral_port_t * const per_port =
                    &__smpc_peripheral_ports[port_idx];

                int32_t connected;

                /* Update peripheral connected directly to the port */
                if ((connected = _peripheral_update(/* parent = */ NULL,
                            per_port->peripheral, port_idx + 1)) < 0) {
                        /* Couldn't parse data; invalid peripheral */
                        per_port->peripheral->connected = 0;
                        _port_peripherals_free(per_port);
                        continue;
                }

                _port_peripherals_free(per_port);

                if (connected > 1) {
                        int32_t sub_port;

                        per_port->peripheral->connected = 0;
                        for (sub_port = 1; connected > 0; connected--, sub_port++) {
                                smpc_peripheral_t *peripheral;

                                peripheral = _peripheral_alloc();
                                assert(peripheral != NULL);

                                if ((_peripheral_update(/* parent = */ per_port, peripheral, sub_port)) < 0) {
                                        _peripheral_free(peripheral);
                                        continue;
                                }

                                /* Add peripheral */
                                TAILQ_INSERT_TAIL(&per_port->peripherals, peripheral, peripherals);

                                per_port->peripheral->connected++;
                        }
                }
        }
}

static void
_port_peripherals_free(smpc_peripheral_port_t *per_port)
{
        assert(per_port != NULL);

        smpc_peripheral_t *peripheral;
        smpc_peripheral_t *tmp_peripheral;

        for (peripheral = TAILQ_FIRST(&per_port->peripherals);
             (peripheral != NULL) && (tmp_peripheral = TAILQ_NEXT(peripheral, peripherals), 1);
             peripheral = tmp_peripheral) {
                TAILQ_REMOVE(&per_port->peripherals, peripheral, peripherals);

                _peripheral_free(peripheral);
        }
}

static smpc_peripheral_t *
_peripheral_alloc(void)
{
        smpc_peripheral_t *peripheral;

        peripheral = memb_alloc(&_peripherals_memb);
        assert(peripheral != NULL);

        /* Ignore all other fields if peripheral is not connected */
        peripheral->connected = 0;

        return peripheral;
}

static void
_peripheral_free(smpc_peripheral_t *peripheral)
{
        assert(peripheral != NULL);

        int ret __unused;
        ret = memb_free(&_peripherals_memb, peripheral);
        assert(ret == 0);
}

static int32_t
_peripheral_update(smpc_peripheral_port_t *per_port_parent,
    smpc_peripheral_t *peripheral, uint8_t port)
{
        uint8_t multitap_id;

        uint32_t connected_count;
        connected_count = 0;

        if (per_port_parent == NULL) {
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
                        connected_count = PC_GET_NUM_CONNECTIONS(_oreg_offset);
                        /* At least two peripheral ports are required */
                        connected_count = (connected_count < MAX_PORTS) ? 0 : connected_count;
                        break;
                case 0x0F:
                        connected_count = PC_GET_NUM_CONNECTIONS(_oreg_offset);
                        /* Only a single peripheral can be directly connected */
                        connected_count = (connected_count > 1) ? 0 : connected_count;
                        break;
                default:
                        connected_count = 0;
                }

                _oreg_offset++;
        }

        if (connected_count == 0) {
                /* Nothing directly connected to the port */
                return -1;
        }

        uint8_t type;
        uint8_t size;
        uint8_t id;

        if (connected_count > 1) {
                peripheral->connected = connected_count;
                peripheral->port = port;
                peripheral->type = multitap_id;
                peripheral->size = 0x00;
                peripheral->parent = per_port_parent;

                return connected_count;
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
        id = PC_GET_ID(_oreg_offset);
        if (size > 0) {
                /* Peripheral data collection #1 */
                /* Check if the ID is valid */
                switch (id) {
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
        case 0x04:
                peripheral->previous_data[0] = peripheral->data[0];
                peripheral->previous_data[1] = peripheral->data[1];
                peripheral->previous_data[2] = peripheral->data[2];
                peripheral->previous_data[3] = peripheral->data[3];

                peripheral->data[0] = PC_GET_DATA_BYTE(_oreg_offset, 0) ^ 0xFF;
                peripheral->data[1] = PC_GET_DATA_BYTE(_oreg_offset, 1) ^ 0xFF;
                peripheral->data[2] = PC_GET_DATA_BYTE(_oreg_offset, 2) ^ 0xFF;
                peripheral->data[3] = PC_GET_DATA_BYTE(_oreg_offset, 3) ^ 0xFF;
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
                peripheral->data[2] = PC_GET_DATA_BYTE(_oreg_offset, 2) ^ 0xFF;
                peripheral->data[3] = PC_GET_DATA_BYTE(_oreg_offset, 3) ^ 0xFF;
                peripheral->data[4] = PC_GET_DATA_BYTE(_oreg_offset, 4) ^ 0xFF;
                peripheral->data[5] = PC_GET_DATA_BYTE(_oreg_offset, 5) ^ 0xFF;
                break;
        default:
                /* XXX
                 * Currently not supporting other sizes */
                assert(false);
        }

        peripheral->connected = 1;
        peripheral->port = port;
        peripheral->type = id;
        peripheral->size = size;
        peripheral->parent = per_port_parent;

        /* Move onto the next peripheral */
        _oreg_offset += size;

        return connected_count;
}

static void
_system_manager_handler(void)
{
        static uint32_t offset = 0;

        /* Prevent a buffer overrun if the user hasn't called to process the
         * collection buffer */
        if (_collection_complete) {
            return;
        }

        assert(offset < sizeof(_oreg_buf));

        /* We don't have much time in the critical section. Just buffer the
         * registers */
        for (uint32_t oreg = 0; oreg < SMPC_OREGS; oreg++, offset++) {
                OREG_SET(offset, MEMORY_READ(8, OREG(oreg)));
        }

        const uint8_t sr = MEMORY_READ(8, SMPC(SR));

        if ((sr & 0x80) == 0x80) {
                if ((sr & NPE) == 0x00) {
                        /* Mark that SMPC status and peripheral data collection
                         * is complete */
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
