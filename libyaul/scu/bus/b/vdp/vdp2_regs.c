/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "vdp-internal.h"

vdp2_registers_t* 
vdp2_regs_get(void)
{
        return _state_vdp2()->regs;
}

vdp2_registers_t
vdp2_regs_get_copy(void)
{
        return *_state_vdp2()->regs;
}