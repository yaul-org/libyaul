/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_mask_get
.type _cpu_intc_mask_get, @function

_cpu_intc_mask_get:
        stc sr, r0
        shlr2 r0
        shlr2 r0
        and #0x0F, r0
        rts
        nop
