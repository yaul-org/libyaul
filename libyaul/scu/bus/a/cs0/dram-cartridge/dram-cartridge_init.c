/*
 * Copyright (c) 2012 Israel Jacques
 *
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <dram-cartridge.h>

#include "dram-cartridge-internal.h"

uint8_t id = 0x00;
void *base = NULL;

static bool detected(void);

void
dram_cartridge_init(void)
{
        uint32_t asr0;
        uint32_t unknown;

        /* Determine ID and base address */
        if (detected())
                return;

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

        /* XXX
         *
         * Add values starting at offset 0x100 to the end of the address
         * space in 16-bit half word unit strides.
         *
         * The checksum is a 32-bit value that is compared against the
         * data cartridge's system ID */
}

static bool
detected(void)
{
        uint32_t read;
        uint32_t write;
        uint32_t b;

        /*               8-Mbit DRAM            32-Mbit DRAM
         *             +--------------------+ +--------------------+
         * 0x224000000 | DRAM #0            | | DRAM #0            |
         *             +--------------------+ |                    |
         * 0x224800000 | DRAM #0 (mirrored) | |                    |
         *             +--------------------+ |                    |
         * 0x225000000 | DRAM #0 (mirrored) | |                    |
         *             +--------------------+ |                    |
         * 0x225800000 | DRAM #0 (mirrored) | |                    |
         *             +--------------------+ +--------------------+
         * 0x226000000 | DRAM #1            | | DRAM #1            |
         *             +--------------------+ |                    |
         * 0x226800000 | DRAM #1 (mirrored) | |                    |
         *             +--------------------+ |                    |
         * 0x227000000 | DRAM #1 (mirrored) | |                    |
         *             +--------------------+ |                    |
         * 0x227800000 | DRAM #1 (mirrored) | |                    |
         * 0x227FFFFFF +--------------------+ +--------------------+
         */

        /* Check the ID */
        id = MEMORY_READ(8, CS1(ID));
        id &= 0xFF;

        if ((id != DRAM_CARTRIDGE_ID_1MIB) &&
            (id != DRAM_CARTRIDGE_ID_4MIB)) {
                id = 0x00;
                return false;
        }

        /* Check DRAM #0 for mirrored banks */
        write = 0x5A5A5A5A;
        MEMORY_WRITE(32, DRAM0(0, 0x00000000), write);
        for (b = 1; b < DRAM_CARTRIDGE_BANKS; b++) {
                read = MEMORY_READ(32, DRAM0(b, 0x00000000));

                /* Is it mirrored? */
                if (read != write)
                        continue;

                /* Thanks to Joe Fenton or the suggestion to return the
                 * last mirrored DRAM #0 bank in order to get a
                 * contiguous address space */
                if (id != DRAM_CARTRIDGE_ID_1MIB)
                        id = DRAM_CARTRIDGE_ID_1MIB;
                base = (void *)DRAM0(3, 0x00000000);
                return true;
        }

        /* 32-Mbit */
        if (id != DRAM_CARTRIDGE_ID_4MIB)
                id = DRAM_CARTRIDGE_ID_4MIB;
        base = (void *)DRAM0(0, 0x00000000);

        return true;
}
