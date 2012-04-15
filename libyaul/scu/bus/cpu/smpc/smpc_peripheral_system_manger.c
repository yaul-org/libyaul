/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <assert.h>

#include <smpc/peripheral.h>

#include "smpc_internal.h"

static void fetch_output_regs(void);

uint8_t offset = 0;
uint8_t oreg_buf[MAX_PORT_DEVICES * MAX_PORT_DATA_SIZE];

void
smpc_peripheral_system_manager(void)
{
        /* Fetch but no parsing */
        fetch_output_regs();

        /* Check if there is any remaining peripheral data */
        if (((MEM_READ(SMPC(SR))) & 0x20) == 0) {
                /* Issue a "BREAK" for the "INTBACK" command */
                MEM_POKE(IREG(0), 0x40);
                return;
        }

        /* Let's not yet cover this case yet */
        assert(0, "We can't fetch more data");

        /* Issue a "CONTINUE" for the "INTBACK" command */
        MEM_POKE(IREG(0), 0x80);
}

static void
fetch_output_regs(void)
{
        uint8_t k;
        uint8_t ooffset;

        for (k = 0; k < SMPC_OREGS; k++) {
                ooffset = k;
                /* We don't have much time in the critical section. Just
                 * buffer the registers */
                OREG_SET(k, MEM_READ(OREG(ooffset)));
        }
}
