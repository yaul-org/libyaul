/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bus/cpu/cpu.h>
#include <smpc/peripheral.h>

#include <assert.h>

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

static void fetch_output_regs(void);

uint8_t oreg_buf[MAX_PORT_DEVICES * MAX_PORT_DATA_SIZE];

void
smpc_peripheral_system_manager(void)
{

        /* Fetch but no parsing */
        fetch_output_regs();

        /* Check if there is any remaining peripheral data */
        if (((MEMORY_READ(8, SMPC(SR))) & NPE) == 0x00) {
                /* Issue a "BREAK" for the "INTBACK" command */
                MEMORY_WRITE(8, IREG(0), 0x40);
                return;
        }

        /* Let's not yet cover this case yet since we can't fetch more data */
        assert(((MEMORY_READ(8, SMPC(SR))) & NPE) == 0x00);

        /* Issue a "CONTINUE" for the "INTBACK" command */
        MEMORY_WRITE(8, IREG(0), 0x80);
}

static void
fetch_output_regs(void)
{
        static uint16_t oboffset = 0;
        uint8_t ooffset;

        offset = 0;

        /* Is this the first time fetching peripheral data? */
        if (((MEMORY_READ(8, SMPC(SR))) & PDL) == PDL)
                oboffset = 0;

        /* What if we exceed our capacity? Buffer overflow */
        assert(oboffset <= 255);

        for (ooffset = 0; ooffset < SMPC_OREGS; ooffset++, oboffset++) {
                /* We don't have much time in the critical section. Just
                 * buffer the registers */
                OREG_SET(oboffset, MEMORY_READ(8, OREG(ooffset)));
        }
}
