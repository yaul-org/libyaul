/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <scu/dma.h>
#include <scu/dsp.h>
#include <scu/timer.h>

#include <scu-internal.h>

void
scu_init(void)
{
        scu_ic_status_set(SCU_IC_IST_NONE);
        scu_ic_mask_set(SCU_IC_MASK_NONE);

        scu_dma_init();
        scu_dsp_init();
        scu_timer_init();
}
