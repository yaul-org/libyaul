/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <stdbool.h>

#include <ic/ic.h>
#include <bus/cpu/cpu.h>
#include <dma/dma.h>
#include <bus/b/vdp1/vdp1.h>
#include <bus/b/vdp2/vdp2.h>

#define RGB(r, g, b)    (0x8000 | ((r) & 0x1f) | (((g) & 0x1f)  << 5) | (((b) & 0x1f) << 10))
#define BLCS_COL(x)     (0x0001fffe + (x))

#define STATUS_ERROR       1
#define STATUS_WAIT        2
#define STATUS_WARNING     3
#define STATUS_OK          4

static void scu_dma_level_0(void);
static void scu_dma_level_1(void);
static void scu_dma_level_2(void);

static void (*dma_level[])(void) = {
        scu_dma_level_0,
        scu_dma_level_1,
        scu_dma_level_2
};

static void set_status(int);
static void scu_dma_illegal(void);
static void scu_dma_level_0_end(void);
static void scu_dma_level_1_end(void);
static void scu_dma_level_2_end(void);

int
main(void)
{
        uint16_t blcs_color[] = {
                RGB(0, 0, 0)
        };

        uint32_t mask;

        vdp2_init();
        vdp2_tvmd_ed_set(); /* Turn display ON */
        vdp2_scrn_blcs_set(/* lcclmd = */ false, 3, BLCS_COL(0), blcs_color);

        scu_dma_cpu_init();

        mask = IC_MSK_LEVEL_0_DMA_END | IC_MSK_LEVEL_1_DMA_END | IC_MSK_LEVEL_2_DMA_END | IC_MSK_DMA_ILLEGAL;
        /* Disable interrupts */
        cpu_intc_vct_disable();
        scu_ic_msk_chg(IC_MSK_ALL, mask);
        scu_ic_vct_set(IC_VCT_LEVEL_0_DMA_END, scu_dma_level_0_end);
        scu_ic_vct_set(IC_VCT_LEVEL_1_DMA_END, scu_dma_level_1_end);
        scu_ic_vct_set(IC_VCT_LEVEL_2_DMA_END, scu_dma_level_2_end);
        scu_ic_vct_set(IC_VCT_DMA_ILLEGAL, scu_dma_illegal);
        scu_ic_msk_chg(IC_MSK_ALL & ~mask, IC_MSK_NULL);
        /* Enable interrupts */
        cpu_intc_vct_enable();

        dma_level[0]();

        for (;;) {
        }

        return 0;
}

static void
set_status(int no)
{
        static uint16_t blcs_color[] = {
                RGB(0, 0, 0), /* Black */
                RGB(31, 0, 0), /* Red */
                RGB(31, 15, 0), /* Orange */
                RGB(31, 31, 0), /* Yellow */
                RGB(0, 31, 0) /* Green */
        };

        vdp2_scrn_blcs_set(/* lcclmd = */ false, 3, BLCS_COL(0), &blcs_color[no]);
}

static void
scu_dma_illegal(void)
{
        set_status(STATUS_ERROR);
}

static void
scu_dma_level_0(void)
{
        struct dma_level_cfg cfg;

        cfg.mode.direct.src = (void *)0x05c00000;
        cfg.mode.direct.dst = (void *)0x05c00000;
        cfg.mode.direct.len = 0x1000;
        cfg.starting_factor = DMA_STRT_FACTOR_ENABLE;
        cfg.add = 4;

        set_status(STATUS_WAIT);
        scu_dma_cpu_level_set(DMA_LEVEL_0, DMA_MODE_DIRECT, &cfg);
        scu_dma_cpu_level_start(DMA_LEVEL_0);
}

static void
scu_dma_level_0_end(void)
{
        set_status(STATUS_OK);
}

static void
scu_dma_level_1(void)
{
        set_status(STATUS_WAIT);
}

static void
scu_dma_level_1_end(void)
{
        set_status(STATUS_OK);
}

static void
scu_dma_level_2(void)
{
        set_status(STATUS_WAIT);
}

static void
scu_dma_level_2_end(void)
{
        set_status(STATUS_OK);
}
