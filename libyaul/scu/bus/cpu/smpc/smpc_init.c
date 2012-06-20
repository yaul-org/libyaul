/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <bus/cpu/cpu.h>
#include <ic/ic.h>
#include <irq-mux.h>
#include <smpc/peripheral.h>
#include <timer/timer.h>
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

        mask = IC_MSK_SYSTEM_MANAGER | IC_MSK_TIMER_0;
        scu_ic_mask_chg(IC_MSK_ALL, mask);

        scu_ic_interrupt_set(IC_VCT_TIMER_0, &smpc_peripheral_data);
        scu_ic_interrupt_set(IC_VCT_SYSTEM_MANAGER, &smpc_peripheral_system_manager);
        scu_ic_mask_chg(IC_MSK_ALL & ~mask, IC_MSK_NULL);

        scu_timer_0_set(5);
        scu_timer_1_set(0);
        scu_timer_1_mode_set(/* sp_line = */ true);

        /* Add to VDP2 VBLANK-OUT mux */
        vbo = vdp2_tvmd_vblank_out_irq_get();
        irq_mux_handle_add(vbo, smpc_peripheral_parse, NULL);

        /* Enable interrupts */
        cpu_intc_enable();
}
