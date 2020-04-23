/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include <cpu/intc.h>
#include <cpu/dmac.h>

#include <smpc/smc.h>

#include <scu/ic.h>

#include <vdp.h>

#include <sys/dma-queue.h>

#include <dbgio.h>

static void _vblank_in_handler(void);

void
_internal_reset(void)
{
        cpu_intc_mask_set(15);
        scu_ic_mask_chg(SCU_IC_MASK_NONE, SCU_IC_MASK_ALL);

        vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
            VDP2_TVMD_VERT_224);
        vdp2_scrn_back_screen_color_set(VDP2_VRAM_ADDR(0, 0x01FFFE),
            COLOR_RGB1555(1, 0, 7, 0));

        vdp2_scrn_display_clear();

        vdp1_env_default_set();

        vdp2_sprite_priority_set(0, 0);
        vdp2_sprite_priority_set(1, 0);
        vdp2_sprite_priority_set(2, 0);
        vdp2_sprite_priority_set(3, 0);
        vdp2_sprite_priority_set(4, 0);
        vdp2_sprite_priority_set(5, 0);
        vdp2_sprite_priority_set(6, 0);
        vdp2_sprite_priority_set(7, 0);

        /* Clear any pending VDP sync user callbacks */
        vdp_sync_user_callback_clear();

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        scu_ic_ihr_clear(SCU_IC_INTERRUPT_VBLANK_IN);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_VBLANK_OUT);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_HBLANK_IN);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_TIMER_0);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_TIMER_1);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_DSP_END);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_SOUND_REQUEST);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_SYSTEM_MANAGER);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_PAD_INTERRUPT);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_LEVEL_2_DMA_END);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_LEVEL_1_DMA_END);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_LEVEL_0_DMA_END);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_DMA_ILLEGAL);
        scu_ic_ihr_clear(SCU_IC_INTERRUPT_SPRITE_END);

        cpu_intc_ihr_clear(CPU_INTC_INTERRUPT_FRT_ICI);
        cpu_intc_ihr_clear(CPU_INTC_INTERRUPT_FRT_OCI);
        cpu_intc_ihr_clear(CPU_INTC_INTERRUPT_FRT_OVI);
        cpu_intc_ihr_clear(CPU_INTC_INTERRUPT_WDT_ITI);
        cpu_intc_ihr_clear(CPU_INTC_INTERRUPT_DMAC0);
        cpu_intc_ihr_clear(CPU_INTC_INTERRUPT_DMAC1);

        dma_queue_clear();

        cpu_dmac_stop();

        smpc_smc_cdoff_call();
        smpc_smc_sshoff_call();

        vdp2_tvmd_display_set();

        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);

        scu_ic_mask_chg(~SCU_IC_MASK_VBLANK_IN, SCU_IC_MASK_NONE);
        cpu_intc_mask_set(14);
}

static void
_vblank_in_handler(void)
{
        /* Synchronize VDP2 only.
         *
         * Avoid using vdp_sync() as we don't need to sync VDP2 and can no
         * longer fire off any interrupts */
        vdp2_sync_commit();
}
