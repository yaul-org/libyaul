/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <dram-cartridge.h>

#include "dram-cartridge-internal.h"

size_t
dram_cartridge_size(void)
{
        switch (id) {
        case DRAM_CARTRIDGE_ID_1MIB:
                return 0x00100000;
        case DRAM_CARTRIDGE_ID_4MIB:
                return 0x00400000;
        default:
                return 0;
        }
}
