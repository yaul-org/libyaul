/*
 * Copyright (c) 2012-2022 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <usb-cart.h>
#include <cd-block.h>

#include "cdfs.h"

void
cdfs_sector_read(sector_t sector, sector_buffer_t *sector_buffer)
{
    cd_block_sector_read(LBA2FAD(sector), sector_buffer);
}
