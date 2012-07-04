/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_mask_set
.type _cpu_intc_mask_set, @function

_cpu_intc_mask_set:
        stc sr, r1
        mov #0xF0, r2
        shll2 r4
        shll2 r4
        and r2, r1
        or r4, r1
        ldc r1, sr
        rts
        nop
