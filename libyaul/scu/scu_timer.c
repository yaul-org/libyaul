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
scu_timer_init(void)
{
        scu_timer_disable();

        scu_timer_t0_clear();
        scu_timer_t1_clear();

        scu_timer_t0_value_set(0);
        scu_timer_t1_value_set(0);
}

void
scu_timer_t0_set(void (*ihr)(void) __unused)
{
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_TIMER_0);

        if (ihr != NULL) {
                scu_ic_mask_chg(~IC_MASK_TIMER_0, IC_MASK_NONE);
        }

        cpu_intc_ihr_set(IC_INTERRUPT_TIMER_0, ihr);
}

void
scu_timer_t1_set(void (*ihr)(void) __unused)
{
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_TIMER_1);

        if (ihr != NULL) {
                scu_ic_mask_chg(~IC_MASK_TIMER_1, IC_MASK_NONE);
        }

        cpu_intc_ihr_set(IC_INTERRUPT_TIMER_1, ihr);
}
