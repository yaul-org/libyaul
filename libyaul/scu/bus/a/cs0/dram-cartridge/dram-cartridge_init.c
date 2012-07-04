/*
 * Copyright (c) 2012 Israel Jacquez
 *
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <dram-cartridge.h>

#include "dram-cartridge-internal.h"

uint8_t id = 0x00;
void *base = NULL;

static bool detected(void);

void
dram_cartridge_init(void)
{
        static bool initialized = false;

        uint32_t asr0;
        uint32_t aref;

        if (initialized)
                return;

        /* Write to A-Bus "dummy" area */
        MEMORY_WRITE(16, DUMMY(UNKNOWN), 0x0001);

        /* Set the SCU wait */
        /* Don't ask about this magic constant */
        asr0 = MEMORY_READ(32, SCU(ASR0));
        MEMORY_WRITE(32, SCU(ASR0), 0x23301FF0);
        /* Write to A-Bus refresh */
        aref = MEMORY_READ(32, SCU(AREF));
        MEMORY_WRITE(32, SCU(AREF), 0x00000013);

        /* Determine ID and base address */
        if (!(detected())) {
                /* Restore values in case we can't detect DRAM
                 * cartridge */
                MEMORY_WRITE(32, SCU(ASR0), asr0);
                MEMORY_WRITE(32, SCU(AREF), aref);
                return;
        }

        initialized = true;
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
                base = NULL;
                return false;
        }

        for (b = 0; b < DRAM_CARTRIDGE_BANKS; b++) {
                MEMORY_WRITE(32, DRAM(0, b, 0x00000000), 0x00000000);
                MEMORY_WRITE(32, DRAM(1, b, 0x00000000), 0x00000000);
        }

        /* Check DRAM #0 for mirrored banks */
        write = 0x5A5A5A5A;
        MEMORY_WRITE(32, DRAM(0, 0, 0x00000000), write);
        for (b = 1; b < DRAM_CARTRIDGE_BANKS; b++) {
                read = MEMORY_READ(32, DRAM(0, b, 0x00000000));

                /* Is it mirrored? */
                if (read != write)
                        continue;

                /* Thanks to Joe Fenton or the suggestion to return the
                 * last mirrored DRAM #0 bank in order to get a
                 * contiguous address space */
                if (id != DRAM_CARTRIDGE_ID_1MIB)
                        id = DRAM_CARTRIDGE_ID_1MIB;
                base = (void *)DRAM(0, 3, 0x00000000);
                return true;
        }

        if (id != DRAM_CARTRIDGE_ID_4MIB)
                id = DRAM_CARTRIDGE_ID_4MIB;
        base = (void *)DRAM(0, 0, 0x00000000);

        return true;
}
