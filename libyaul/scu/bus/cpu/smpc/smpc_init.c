/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <ic/ic.h>
#include <bus/cpu/cpu.h>
#include <smpc/peripheral.h>

#include "smpc_internal.h"

void
smpc_init(void)
{
        uint32_t mask;

        /* Set both ports to "SMPC" control mode. */
        MEM_POKE(SMPC(EXLE1), 0x00);
        MEM_POKE(SMPC(IOSEL1), 0x00);
        MEM_POKE(SMPC(DDR1), 0x00);
        MEM_POKE(SMPC(PDR1), 0x00);

        /* Disable interrupts */
        cpu_intc_vct_disable();

        mask = IC_MSK_SYSTEM_MANAGER;
        scu_ic_msk_chg(IC_MSK_ALL, mask);

        scu_ic_vct_set(IC_VCT_SYSTEM_MANAGER, &smpc_peripheral_system_manager);
        scu_ic_msk_chg(IC_MSK_ALL & ~mask, IC_MSK_NULL);

        /* Enable interrupts */
        cpu_intc_vct_enable();
}
