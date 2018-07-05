/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <stdio.h>

#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/frt.h>

void
cpu_frt_init(uint8_t clock_div)
{
        /* Disable interrupts */
        uint32_t i_mask;
        i_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(0x0F);

        /* Disable FRT related interrupts */
        MEMORY_WRITE(16, CPU(IPRB), 0x0F << 8);

        /* Set time control register */
        MEMORY_WRITE_AND(8, CPU(TCR), ~0x03 | (clock_div & 0x03));

        /* Reset status */
        MEMORY_WRITE(8, CPU(FTCSR), 0x00);

        /* Always set bit 0 */
        MEMORY_WRITE(8, CPU(TIER), 0x01);

        /* Always set bits 7-5 */
        MEMORY_WRITE(8, CPU(TOCR), 0xE0);

        cpu_intc_ihr_set(INTC_INTERRUPT_FRT_ICI, NULL);
        cpu_intc_ihr_set(INTC_INTERRUPT_FRT_OCI, NULL);
        cpu_intc_ihr_set(INTC_INTERRUPT_FRT_OVI, NULL);

        /* Enable interrupts */
        cpu_intc_mask_set(i_mask);
}
