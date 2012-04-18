/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <assert.h>

#include <smpc/peripheral.h>

#include "smpc_internal.h"

#define P1MD0   0x00 /* 15B mode */
#define P1MD1   0x02 /* 255B mode */
#define P1MD2   0x03 /* 0B mode (port is not accessed) */
#define P2MD0   0x00
#define P2MD1   0x04
#define P2MD2   0x0C
#define RESB    0x10
#define NPE     0x20
#define PDL     0x40

static void fetch_output_regs(void);

uint8_t oreg_buf[MAX_PORT_DEVICES * MAX_PORT_DATA_SIZE];

void
smpc_peripheral_system_manager(void)
{
        /* Fetch but no parsing */
        fetch_output_regs();

        /* Check if there is any remaining peripheral data */
        if (((MEM_READ(SMPC(SR))) & NPE) == 0x00) {
                /* Issue a "BREAK" for the "INTBACK" command */
                MEM_POKE(IREG(0), 0x40);
                return;
        }

        /* Let's not yet cover this case yet */
        assert(0, "we can't fetch more data");

        /* Issue a "CONTINUE" for the "INTBACK" command */
        MEM_POKE(IREG(0), 0x80);
}

static void
fetch_output_regs(void)
{
        static uint16_t oboffset = 0;
        uint8_t ooffset;

        offset = 0;

        /* Is this the first time fetching peripheral data? */
        if (((MEM_READ(SMPC(SR))) & PDL) == PDL)
                oboffset = 0;

        /* What if we exceed our capacity? */
        assert(oboffset <= 255, "buffer overflow");

        for (ooffset = 0; ooffset < SMPC_OREGS; ooffset++, oboffset++) {
                /* We don't have much time in the critical section. Just
                 * buffer the registers */
                OREG_SET(oboffset, MEM_READ(OREG(ooffset)));
        }
}
