/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_clock_freq_get
.type _cpu_clock_freq_get, @function

_cpu_clock_freq_get:
        mov.l .LC1, r1
        mov.l @r1, r0
        rts
        nop

.align 2

.LC1:
.long 0x06000324
