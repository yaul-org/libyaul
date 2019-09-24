/*
 * Copyright (c) 2012-2019 Israel Jacquez
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

#define CPU_DMAC_IHR_INDEX_CH0  0
#define CPU_DMAC_IHR_INDEX_CH1  1

typedef void (*ihr_entry_t)(void);

static ihr_entry_t _master_dmac_ihr_table[] = {
        _default_ihr,
        _default_ihr
};

static ihr_entry_t _slave_dmac_ihr_table[] = {
        _default_ihr,
        _default_ihr
};

static ihr_entry_t* _dmac_ihr_table_get(void);

void
cpu_dmac_init(void)
{
        cpu_dmac_disable();
        cpu_dmac_channel_stop(0);
        cpu_dmac_channel_stop(1);

        MEMORY_WRITE(32, CPU(VCRDMA0), CPU_INTC_INTERRUPT_DMAC0);
        MEMORY_WRITE(32, CPU(VCRDMA1), CPU_INTC_INTERRUPT_DMAC1);

        cpu_dmac_interrupt_priority_set(15);

        MEMORY_WRITE(8, CPU(DRCR0), 0x00);
        MEMORY_WRITE(8, CPU(DRCR1), 0x00);

        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC0, _dmac_ch0_ihr_handler);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC1, _dmac_ch1_ihr_handler);

        cpu_dmac_enable();
}

void
cpu_dmac_status_get(struct dmac_status *status)
{
        if (status == NULL) {
                return;
        }

        uint32_t reg_dmaor;
        reg_dmaor = MEMORY_READ(32, CPU(DMAOR));

        status->enabled = reg_dmaor & 0x00000001;
        status->priority_mode = (reg_dmaor >> 3) & 0x00000001;
        status->address_error = (reg_dmaor >> 2) & 0x00000001;
        status->nmi_interrupt = (reg_dmaor >> 1) & 0x00000001;

        uint32_t reg_chcr0;
        reg_chcr0 = MEMORY_READ(32, CPU(CHCR0));
        uint32_t reg_chcr1;
        reg_chcr1 = MEMORY_READ(32, CPU(CHCR1));

        uint8_t ch0_enabled;
        ch0_enabled = reg_chcr0 & 0x00000001;
        uint8_t ch1_enabled;
        ch1_enabled = reg_chcr1 & 0x00000001;

        status->channel_enabled = (ch1_enabled << 1) | ch0_enabled;

        /*-
         * Truth table to determine if a specific channel is enabled or not
         *
         * TE DE Busy
         *  0  0 0
         *  0  1 1    Could be a false positive
         *  1  0 0
         *  1  1 0
         */
        uint8_t de;
        uint8_t te;

        uint8_t ch0_busy;
        de = reg_chcr0 & 0x00000001;
        te = (reg_chcr0 >> 1) & 0x00000001;
        ch0_busy = (((de | te) ^ 1) | de) ^ 1;

        uint8_t ch1_busy;
        de = reg_chcr1 & 0x00000001;
        te = (reg_chcr1 >> 1) & 0x00000001;
        ch1_busy = (((de | te) ^ 1) | de) ^ 1;

        status->channel_busy = (ch1_busy << 1) | ch0_busy;
}

void
cpu_dmac_channel_config_set(const struct cpu_dmac_cfg *cfg)
{
        uint32_t n;
        n = (cfg->channel & 0x01) << 4;

        cpu_dmac_channel_stop(cfg->channel);

        /* Source and destination address modes */
        uint32_t reg_chcr;
        reg_chcr = ((cfg->src_mode & 0x03) << 12) |
                   ((cfg->dst_mode & 0x03) << 14) |
                   ((cfg->stride & 0x03) << 10) |
                   ((cfg->bus_mode & 0x01) << 4) |
                   /* Always enable AR (auto-request mode) */
                   0x00000200;

        uint32_t reg_tcr;
        reg_tcr = cfg->len;

        uint8_t stride;
        stride = cfg->stride & 0x03;

        /* Check that the source and destination addresses are
         * stride-byte aligned */

        if (stride >= CPU_DMAC_STRIDE_4_BYTES) {
                reg_tcr >>= 2;
        } else if (stride == CPU_DMAC_STRIDE_2_BYTES) {
                reg_tcr >>= 1;
        }

        /* Transfer 16MiB inclusive when TCR0 is 0x00000000 */
        reg_tcr = (reg_tcr > 0x00FFFFFF) ? 0x00000000 : reg_tcr;

        ihr_entry_t *dmac_ihr_table;
        dmac_ihr_table = _dmac_ihr_table_get();

        dmac_ihr_table[cfg->channel] = _default_ihr;

        if (cfg->ihr != NULL) {
                /* Enable interrupt */
                reg_chcr |= 0x00000004;

                /* Set interrupt handling routine */
                dmac_ihr_table[cfg->channel] = cfg->ihr;
        }

        MEMORY_WRITE(32, CPU(DAR0 | n), (uint32_t)cfg->dst);
        MEMORY_WRITE(32, CPU(SAR0 | n), (uint32_t)cfg->src);
        MEMORY_WRITE(32, CPU(TCR0 | n), reg_tcr);
        MEMORY_WRITE(32, CPU(CHCR0 | n), reg_chcr);
}

void
cpu_dmac_channel_wait(uint8_t ch)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        /* Don't wait if DMAC is disabled */
        if ((MEMORY_READ(32, CPU(DMAOR)) & 0x00000001) == 0x00000000) {
                return;
        }

        /* Don't wait if the channel isn't enabled */
        if ((MEMORY_READ(32, CPU(CHCR0 | n)) & 0x00000001) == 0x00000000) {
                return;
        }

        /* TE bit will always be set upon normal or abnormal transfer */
        while ((MEMORY_READ(32, CPU(CHCR0 | n)) & 0x00000002) == 0x00000000);
}

static ihr_entry_t *
_dmac_ihr_table_get(void)
{
        const uint8_t which_cpu = cpu_dual_executor_get();

        switch (which_cpu) {
                case CPU_MASTER:
                        return _master_dmac_ihr_table;
                case CPU_SLAVE:
                        return _slave_dmac_ihr_table;
        }

        return NULL;
}

static void __interrupt_handler
_dmac_ch0_ihr_handler(void)
{
        ihr_entry_t *dmac_ihr_table;
        dmac_ihr_table = _dmac_ihr_table_get();

        dmac_ihr_table[CPU_DMAC_IHR_INDEX_CH0]();

        MEMORY_WRITE_AND(32, CPU(CHCR0), ~0x00000005);
}

static void __interrupt_handler
_dmac_ch1_ihr_handler(void)
{
        ihr_entry_t *dmac_ihr_table;
        dmac_ihr_table = _dmac_ihr_table_get();

        dmac_ihr_table[CPU_DMAC_IHR_INDEX_CH1]();

        MEMORY_WRITE_AND(32, CPU(CHCR1), ~0x00000005);
}

static void
_default_ihr(void)
{
}
