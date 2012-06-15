/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _arp_function_handle
.type _arp_function_handle, @function

_arp_function_handle:
        /* Disable interrupts */
        mov #0xF0, r1
        ldc r1, sr
        mov.l r14, @-r15
        sts.l pr, @-r15

        mov.l .LC0, r1
        mov.l @r1, r1
        mov.l .LC1, r4
        jsr @r1

        /* Clear ARP user callback */
        mov #0x00, r1
        mov.l .LC1, r4
        mov.l r1, @(0x00, r4)
        mov.l r1, @(0x04, r4)
        mov.l r1, @(0x08, r4)

        /* Enable interrupts */
        stc sr, r2
        mov #0xFF, r3
        mov #0x0F, r1
        shll8 r3
        or r3, r1
        and r1, r2
        ldc r2, sr
        lds.l @r15+, pr
        mov.l @r15+, r14
        rte
        nop

.align 2

.LC0:
        .long _arp_cb
.LC1:
        .long _arp_callback
