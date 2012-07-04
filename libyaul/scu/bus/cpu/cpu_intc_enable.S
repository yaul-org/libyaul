/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_enable
.type _cpu_intc_enable, @function

_cpu_intc_enable:
        mov #0xFF, r2
        mov #0x0F, r1
        shll8 r2
        stc sr, r3
        or r2, r1
        and r1, r3
        ldc r3, sr
        rts
        nop
