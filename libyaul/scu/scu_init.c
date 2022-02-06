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
__scu_init(void)
{
        scu_ic_status_set(SCU_IC_IST_NONE);
        scu_ic_mask_set(SCU_IC_MASK_NONE);

        __scu_dma_init();
        __scu_dsp_init();
        __scu_timer_init();

        scu_ic_mask_set(SCU_IC_MASK_HBLANK_IN);
}
