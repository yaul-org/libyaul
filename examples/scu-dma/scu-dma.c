/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>

#include <bus/cpu/cpu.h>
#include <dma/dma.h>
#include <ic/ic.h>
#include <smpc.h>
#include <smpc/peripheral.h>
#include <smpc/smc.h>
#include <vdp2.h>
#include <vdp2/tvmd.h>

#include <cons/vdp2.h>

#include <stdio.h>

#define RGB(r, g, b)    (0x8000 | ((r) & 0x1F) | (((g) & 0x1F)  << 5) | (((b) & 0x1F) << 10))

static struct {
        bool st_begin;
#define ST_STATUS_WAIT          0
#define ST_STATUS_END           1
#define ST_STATUS_ILLEGAL       2
        int st_status;
#define ST_LEVEL_0              0
#define ST_LEVEL_1              1
#define ST_LEVEL_2              2
        struct {
#define ST_LEVEL_SF_DISABLE     0
#define ST_LEVEL_SF_ENABLE      1
#define ST_LEVEL_SF_VBLANK_IN   2
#define ST_LEVEL_SF_VBLANK_OUT  3
#define ST_LEVEL_SF_HBLANK_IN   4
                int level_sf;

#define ST_LEVEL_MODE_DIRECT    0
#define ST_LEVEL_MODE_INDIRECT  1
                int level_mode;
        } st_level[3];
} state = {
        .st_begin = false,
        .st_status = ST_STATUS_WAIT,
        .st_level = {
                {
                        .level_sf = ST_LEVEL_SF_DISABLE,
                        .level_mode = ST_LEVEL_MODE_DIRECT
                }, {
                        .level_sf = ST_LEVEL_SF_DISABLE,
                        .level_mode = ST_LEVEL_MODE_DIRECT
                }, {
                        .level_sf = ST_LEVEL_SF_DISABLE,
                        .level_mode = ST_LEVEL_MODE_DIRECT
                }
        }
};

static void display_menu(void);

static void scu_dma_illegal(void);
static void scu_dma_level(int);
static void scu_dma_level_0_end(void);
static void scu_dma_level_1_end(void);
static void scu_dma_level_2_end(void);

static struct cons cons;

int
main(void)
{
        uint16_t blcs_color[] = {
                RGB(0, 7, 7)
        };

        uint16_t mask;

        struct smpc_peripheral_digital *digital;
        int level;

        vdp2_init();
        vdp2_tvmd_display_set(); /* Turn display ON */
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();
        scu_dma_cpu_init();

        cons_vdp2_init(&cons);

        mask = IC_MSK_LEVEL_0_DMA_END | IC_MSK_LEVEL_1_DMA_END | IC_MSK_LEVEL_2_DMA_END | IC_MSK_DMA_ILLEGAL;
        /* Disable interrupts */
        cpu_intc_disable();
        scu_ic_mask_chg(IC_MSK_ALL, mask);
        scu_ic_interrupt_set(IC_VCT_LEVEL_0_DMA_END, scu_dma_level_0_end);
        scu_ic_interrupt_set(IC_VCT_LEVEL_1_DMA_END, scu_dma_level_1_end);
        scu_ic_interrupt_set(IC_VCT_LEVEL_2_DMA_END, scu_dma_level_2_end);
        scu_ic_interrupt_set(IC_VCT_DMA_ILLEGAL, scu_dma_illegal);
        scu_ic_mask_chg(IC_MSK_ALL & ~mask, IC_MSK_NULL);
        /* Enable interrupts */
        cpu_intc_enable();

        digital = smpc_peripheral_digital_port(1);

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                if (!state.st_begin) {
                        if (!digital->connected)
                                continue;

                        display_menu();

                        if (!digital->button.a_trg)
                                level = ST_LEVEL_0;
                        else if (!digital->button.b_trg)
                                level = ST_LEVEL_1;
                        else if (!digital->button.c_trg)
                                level = ST_LEVEL_2;
                        else if (!digital->button.start) {
                                state.st_begin = true;
                                continue;
                        } else {
                                level = -1;
                                continue;
                        }

                        if (!digital->button.l_trg)
                                state.st_level[level].level_mode = ST_LEVEL_MODE_DIRECT;
                        else if (!digital->button.r_trg)
                                state.st_level[level].level_mode = ST_LEVEL_MODE_INDIRECT;
                        else if (!digital->button.x_trg)
                                state.st_level[level].level_sf = ST_LEVEL_SF_VBLANK_IN;
                        else if (!digital->button.y_trg)
                                state.st_level[level].level_sf = ST_LEVEL_SF_VBLANK_OUT;
                        else if (!digital->button.z_trg)
                                state.st_level[level].level_sf = ST_LEVEL_SF_HBLANK_IN;
                        else
                                state.st_level[level].level_sf = ST_LEVEL_SF_ENABLE;

                        continue;
                }

                scu_dma_level(level);

                switch (state.st_status) {
                case ST_STATUS_WAIT:
                        continue;
                case ST_STATUS_ILLEGAL:
                        assert(state.st_status != ST_STATUS_ILLEGAL);
                        continue;
                case ST_STATUS_END:
                        state.st_begin = true;
                        state.st_status = ST_STATUS_WAIT;
                        break;
                }
        }

        return 0;
}

static void
scu_dma_illegal(void)
{
}

static void
display_menu(void)
{
        /* Menu */
        cons_write(&cons, "\n[1;44m       *** SCU DMA Test Menu ***        [m\n\n"
            "[1;44mSCU DMA Level 0[m\n"
            "\tA+L - Mode: Direct\n"
            "\tA+R - Mode: Indirect\n"
            "\tA+X - SF: VBLANK-IN\n"
            "\tA+Y - SF: VBLANK-OUT\n"
            "\tA+Z - SF: HBLANK-IN\n"
            "\n"
            "[1;44mSCU DMA Level 1[m\n"
            "\tB+L - Mode: Direct\n"
            "\tB+R - Mode: Indirect\n"
            "\tB+X - SF: VBLANK-IN\n"
            "\tB+Y - SF: VBLANK-OUT\n"
            "\tB+Z - SF: HBLANK-IN\n"
            "\n"
            "[1;44mSCU DMA Level 2[m\n"
            "\tC+L - Mode: Direct\n"
            "\tC+R - Mode: Indirect\n"
            "\tC+X - SF: VBLANK-IN\n"
            "\tC+Y - SF: VBLANK-OUT\n"
            "\tC+Z - SF: HBLANK-IN\n"
            "[H");
}

static void
scu_dma_level(int level __attribute__ ((unused)))
{
        struct dma_level_cfg cfg;

        cfg.mode.direct.src = (void *)0x06040000;
        cfg.mode.direct.dst = (void *)0x05C00000;
        cfg.mode.direct.len = 0x1000;
        cfg.starting_factor = DMA_MODE_START_FACTOR_ENABLE;
        cfg.add = 4;

        scu_dma_cpu_level_set(DMA_LEVEL_0, DMA_MODE_DIRECT, &cfg);
        scu_dma_cpu_level_start(DMA_LEVEL_0);
}

static void
scu_dma_level_0_end(void)
{

        state.st_status = ST_STATUS_END;
}

static void
scu_dma_level_1_end(void)
{

        state.st_status = ST_STATUS_END;
}

static void
scu_dma_level_2_end(void)
{

        state.st_status = ST_STATUS_END;
}
