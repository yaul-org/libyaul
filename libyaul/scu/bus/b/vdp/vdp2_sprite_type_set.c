/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp-internal.h"

void
vdp2_sprite_type_set(uint8_t type)
{
#ifdef DEBUG
        assert((type >= 0x00) && (type <= 0x0F));

        /* XXX: Check if the sprite color format is valid with the
         * sprite type specified. */
#endif /* DEBUG */

        /* Types 0 to 7 are for low resolution (320 or 352), and types 8
         * to 15 are for high resolution (640 or 704).
         *
         * The data widths are 16-bits and 8-bits, respectively. */
        _state_vdp2()->regs.spctl &= 0xFFF0;
        _state_vdp2()->regs.spctl |= type & 0x000F;
}
