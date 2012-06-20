/*
 * Copyright (c) 2012 Israel Jacques
 *
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <dram-cartridge.h>

#include "dram-cartridge-internal.h"

uint32_t id;

void
dram_cartridge_init(void)
{
        uint32_t asr0;
        uint32_t unknown;

        /* Check the ID */
        id = MEMORY_READ(32, CS1(ID));
        id &= 0x000000FF;
        switch (id) {
        case 0x5A:
                /* 16-Mbit (1MiB) */
                id = 0x00080000;
                break;
        case 0x5C:
                /* 32-Mbit (4MiB) */
                id = 0x00200000;
                break;
        default:
                id = 0x00000000;
                return;
        }

        /* Write to A-Bus "dummy" area */
        unknown = MEMORY_READ(32, DUMMY(UNKNOWN));
        unknown &= 0xFFFF0000;
        unknown |= 0x0001;
        MEMORY_WRITE(32, DUMMY(UNKNOWN), unknown);

        /* Set the SCU wait */
        /* Don't ask about this magic constant */
        asr0 = 0x23301FF0;
        MEMORY_WRITE(32, SCU(ASR0), asr0);
        /* Write to A-Bus refresh */
        MEMORY_WRITE(32, SCU(AREF), 0x00000013);

        /* Add values starting at offset 0x100 to the end of the address
         * space in 16-bit half word unit strides.
         *
         * The checksum is a 32-bit value that is compared against the
         * data cartridge's system ID */
}
