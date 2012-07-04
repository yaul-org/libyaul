/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_disable
.type _cpu_intc_disable, @function

_cpu_intc_disable:
        mov #0xF0, r1
        extu.b r1, r1
        stc sr, r2
        or r2, r1
        ldc r1, sr
        rts
        nop
