/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <cpu/intc.h>

#include <scu/timer.h>
#include <scu/ic.h>

void
__scu_timer_init(void)
{
        scu_timer_disable();

        scu_timer_t0_clear();
        scu_timer_t1_clear();

        scu_timer_t0_value_set(0);
        scu_timer_t1_value_set(0);
}

void
scu_timer_t0_set(scu_timer_ihr_t ihr)
{
        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_IC_MASK_TIMER_0);

        if (ihr != NULL) {
                scu_ic_mask_chg(~SCU_IC_MASK_TIMER_0, SCU_IC_MASK_NONE);
        }

        cpu_intc_ihr_set(SCU_IC_INTERRUPT_TIMER_0, ihr);
}

void
scu_timer_t1_set(scu_timer_ihr_t ihr)
{
        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_IC_MASK_TIMER_1);

        if (ihr != NULL) {
                scu_ic_mask_chg(~SCU_IC_MASK_TIMER_1, SCU_IC_MASK_NONE);
        }

        cpu_intc_ihr_set(SCU_IC_INTERRUPT_TIMER_1, ihr);
}
