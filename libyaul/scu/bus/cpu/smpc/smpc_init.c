/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu.h>
#include <irq-mux.h>
#include <scu/ic.h>
#include <scu/timer.h>
#include <smpc/peripheral.h>
#include <vdp2.h>

#include "smpc-internal.h"

void
smpc_init(void)
{
        irq_mux_t *vbo;
        uint32_t mask;

        /* Set both ports to "SMPC" control mode. */
        MEMORY_WRITE(8, SMPC(EXLE1), 0x00);
        MEMORY_WRITE(8, SMPC(IOSEL1), 0x00);
        MEMORY_WRITE(8, SMPC(DDR1), 0x00);
        MEMORY_WRITE(8, SMPC(PDR1), 0x00);

        /* Disable interrupts */
        cpu_intc_disable();

        mask = IC_MASK_SYSTEM_MANAGER | IC_MASK_TIMER_0;
        scu_ic_mask_chg(IC_MASK_ALL, mask);

        scu_ic_interrupt_set(IC_INTERRUPT_TIMER_0,
            &smpc_peripheral_data);
        scu_ic_interrupt_set(IC_INTERRUPT_SYSTEM_MANAGER,
            &smpc_peripheral_system_manager);
        scu_ic_mask_chg(IC_MASK_ALL & ~mask, IC_MASK_NONE);

        scu_timer_0_set(5);
        scu_timer_1_set(0);
        scu_timer_1_mode_set(/* sp_line = */ true);

        /* Add to VDP2 VBLANK-OUT mux */
        vbo = vdp2_tvmd_vblank_out_irq_get();
        irq_mux_handle_add(vbo, smpc_peripheral_parse, NULL);

        /* Enable interrupts */
        cpu_intc_enable();
}
