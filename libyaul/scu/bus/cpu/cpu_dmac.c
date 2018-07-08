/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>
#include <cpu/dmac.h>
#include <cpu/intc.h>
#include <scu/ic.h>

#include <assert.h>

#include "cpu-internal.h"

static void _dmac_ch0_ihr_handler(void);
static void _dmac_ch1_ihr_handler(void);

static void _default_ihr(void);

#define DMAC_IHR_INDEX_CH0      0
#define DMAC_IHR_INDEX_CH1      1

static void (*_frt_dmac_ihr_table[])(void) = {
        _default_ihr,
        _default_ihr
};

void
cpu_dmac_init(void)
{
        cpu_dmac_disable();
        cpu_dmac_channel_stop(0);
        cpu_dmac_channel_stop(1);

        cpu_dmac_interrupt_priority_set(15);

        MEMORY_WRITE(8, CPU(DRCR0), 0x00);
        MEMORY_WRITE(8, CPU(DRCR1), 0x00);

        cpu_intc_ihr_set(INTC_INTERRUPT_DMAC0, _dmac_ch0_ihr_handler);
        cpu_intc_ihr_set(INTC_INTERRUPT_DMAC1, _dmac_ch1_ihr_handler);

        cpu_dmac_enable();
}

void
cpu_dmac_channel_config_set(const struct dmac_ch_cfg *cfg)
{
        uint32_t n;
        n = (cfg->ccc_ch & 0x01) << 4;

        cpu_dmac_channel_stop(cfg->ccc_ch);

        /* Source and destination address modes */
        uint32_t reg_chcr;
        reg_chcr = ((cfg->ccc_src_mode & 0x03) << 12) |
                   ((cfg->ccc_dst_mode & 0x03) << 14) |
                   ((cfg->ccc_stride & 0x03) << 10) |
                   /* Always enable AR (auto-request mode) */
                   0x00000200;

        uint32_t reg_tcr;
        reg_tcr = cfg->ccc_len;

        switch (cfg->ccc_stride & 0x03) {
        case CPU_DMAC_STRIDE_1_BYTE:
                break;
        case CPU_DMAC_STRIDE_2_BYTES:
                reg_tcr >>= 1;
                break;
        case CPU_DMAC_STRIDE_4_BYTES:
                reg_tcr >>= 2;
                break;
        case CPU_DMAC_STRIDE_16_BYTES:
                reg_tcr <<= 2;
                break;
        }

        /* Transfer 16MiB inclusive when TCR0 is 0x00000000 */
        reg_tcr = (reg_tcr > 0x00FFFFFF) ? 0x00000000 : reg_tcr;

        _frt_dmac_ihr_table[cfg->ccc_ch] = _default_ihr;

        if (cfg->ccc_ihr != NULL) {
                /* Enable interrupt */
                reg_chcr |= 0x00000004;

                /* Set interrupt handling routine */
                _frt_dmac_ihr_table[cfg->ccc_ch] = cfg->ccc_ihr;
        }

        MEMORY_WRITE(32, CPU(DAR0 | n), (uint32_t)cfg->ccc_dst);
        MEMORY_WRITE(32, CPU(SAR0 | n), (uint32_t)cfg->ccc_src);
        MEMORY_WRITE(32, CPU(TCR0 | n), reg_tcr);
        MEMORY_WRITE(32, CPU(CHCR0 | n), reg_chcr);
}

void
cpu_dmac_channel_status_get(struct dmac_ch_status *status __unused)
{
}

static void __attribute__ ((interrupt_handler))
_dmac_ch0_ihr_handler(void)
{
        _frt_dmac_ihr_table[DMAC_IHR_INDEX_CH0]();

        MEMORY_WRITE_AND(32, CPU(CHCR0), ~0x00000004);
}

static void __attribute__ ((interrupt_handler))
_dmac_ch1_ihr_handler(void)
{
        _frt_dmac_ihr_table[DMAC_IHR_INDEX_CH1]();

        MEMORY_WRITE_AND(32, CPU(CHCR1), ~0x00000004);
}

static void
_default_ihr(void)
{
}
