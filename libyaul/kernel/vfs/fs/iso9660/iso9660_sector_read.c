/*
 * Copyright (c) 2012-2022 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <usb-cart.h>
#include <cd-block.h>

#include "iso9660.h"

void
iso9660_sector_read(sector_t sector, void *ptr)
{
        cd_block_sector_read(LBA2FAD(sector), ptr);
}
