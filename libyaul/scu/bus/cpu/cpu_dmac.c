/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>
#include <cpu/dmac.h>
#include <cpu/intc.h>
#include <cpu/cache.h>
#include <scu/ic.h>

#include <sys/callback-list.h>

#include <sys/cdefs.h>

#include "cpu-internal.h"

static void _master_ch0_ihr_handler(void);
static void _slave_ch0_ihr_handler(void);

static void _master_ch1_ihr_handler(void);
static void _slave_ch1_ihr_handler(void);

static callback_t *_ihr_callback_get(void);

#define IHR_INDEX_CH0 0
#define IHR_INDEX_CH1 1

static callback_t _master_ihr_callbacks[2];
static callback_t _slave_ihr_callbacks[2];

static callback_t * const _ihr_callback_tables[] = {
        _master_ihr_callbacks,
        _slave_ihr_callbacks
};

void
__cpu_dmac_init(void)
{
        cpu_dmac_channel_wait(0);
        cpu_dmac_channel_wait(1);
        cpu_dmac_channel_stop(0);
        cpu_dmac_channel_stop(1);
        cpu_dmac_disable();

        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        cpu_ioregs->vcrdma0 = CPU_INTC_INTERRUPT_DMAC0;
        cpu_ioregs->vcrdma1 = CPU_INTC_INTERRUPT_DMAC1;

        cpu_dmac_interrupt_priority_set(15);

        cpu_ioregs->drcr0 = 0x00;
        cpu_ioregs->drcr1 = 0x00;

        const cpu_which_t which_cpu = cpu_dual_executor_get();

        if (which_cpu == CPU_MASTER) {
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC0, _master_ch0_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC1, _master_ch1_ihr_handler);

                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC0 + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _slave_ch0_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC1 + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _slave_ch1_ihr_handler);

                callback_init(&_master_ihr_callbacks[0]);
                callback_init(&_master_ihr_callbacks[1]);
        } else {
                callback_init(&_slave_ihr_callbacks[0]);
                callback_init(&_slave_ihr_callbacks[1]);
        }

        cpu_dmac_enable();
}

void
cpu_dmac_status_get(cpu_dmac_status_t *status)
{
        if (status == NULL) {
                return;
        }

        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        const uint32_t reg_dmaor = cpu_ioregs->dmaor;

        status->enabled = reg_dmaor & 0x00000001;
        status->priority_mode = (reg_dmaor >> 3) & 0x00000001;
        status->address_error = (reg_dmaor >> 2) & 0x00000001;
        status->nmi_interrupt = (reg_dmaor >> 1) & 0x00000001;

        const uint32_t reg_chcr0 = cpu_ioregs->chcr0;
        const uint32_t reg_chcr1 = cpu_ioregs->chcr1;

        const uint8_t ch0_enabled = reg_chcr0 & 0x00000001;
        const uint8_t ch1_enabled = reg_chcr1 & 0x00000001;

        status->channel_enabled = (ch1_enabled << 1) | ch0_enabled;

        /*-
         * Truth table to determine if a specific channel is enabled or not
         *
         * TE DE Busy
         *  0 0  0
         *  0 1  1    Could be a false positive
         *  1 0  0
         *  1 1  0
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
cpu_dmac_channel_config_set(const cpu_dmac_cfg_t *cfg)
{
        const uint32_t channel = cfg->channel & 0x01;

        cpu_dmac_channel_stop(cfg->channel);

        /* Source and destination address modes */
        uint32_t reg_chcr;
        reg_chcr = ((cfg->src_mode & 0x03) << 12) |
                   ((cfg->dst_mode & 0x03) << 14) |
                   ((cfg->stride & 0x03) << 10) |
                   ((cfg->bus_mode & 0x01) << 4);

        if (cfg->non_default) {
                /* Non-default DMAC settings */
                reg_chcr |= ((cfg->request_mode & 0x01) << 9) |
                            ((cfg->dack_mode & 0x01) << 8) |
                            ((cfg->dack_level & 0x01) << 7) |
                            ((cfg->detect_mode & 0x01) << 6) |
                            ((cfg->dreq_level & 0x01) << 5);
        } else {
                /* Default DMAC settings, enable AR (auto-request mode) */
                reg_chcr |= 0x00000200;
        }

        uint32_t reg_tcr;
        reg_tcr = cfg->len;

        const uint8_t reg_drcr = (cfg->resource_select & 0x03);

        uint8_t stride;
        stride = cfg->stride & 0x03;

        /* Check that the source and destination addresses are stride-byte
         * aligned */

        if (stride == CPU_DMAC_STRIDE_16_BYTES) {
                if (reg_tcr > 0x00FFFFFF) {
                        /* Transfer 16MiB inclusive when TCR0 is 0x00000000 */
                        reg_tcr = 0x00000000;
                }

                reg_tcr >>= 2;

                /* During 16-byte transfers, the transfer address mode bit for
                 * dual address mode */
                reg_chcr &= ~0x00000008;
        } else if (stride >= CPU_DMAC_STRIDE_4_BYTES) {
                reg_tcr >>= 2;
        } else if (stride == CPU_DMAC_STRIDE_2_BYTES) {
                reg_tcr >>= 1;
        }

        if (cfg->ihr != NULL) {
                /* Enable interrupt */
                reg_chcr |= 0x00000004;

                callback_t * const ihr_callbacks = _ihr_callback_get();
                callback_t * const ihr_callback =
                    &ihr_callbacks[cfg->channel];

                /* Set interrupt handling routine */
                callback_set(ihr_callback, cfg->ihr, cfg->ihr_work);
        }

        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        switch (channel) {
        case 0:
                cpu_ioregs->dar0 = cfg->dst;
                cpu_ioregs->sar0 = cfg->src;
                cpu_ioregs->tcr0 = reg_tcr;
                cpu_ioregs->chcr0 = reg_chcr;
                cpu_ioregs->drcr0 = reg_drcr;
                break;
        case 1:
                cpu_ioregs->dar1 = cfg->dst;
                cpu_ioregs->sar1 = cfg->src;
                cpu_ioregs->tcr1 = reg_tcr;
                cpu_ioregs->chcr1 = reg_chcr;
                cpu_ioregs->drcr1 = reg_drcr;
                break;
        }
}

void
cpu_dmac_channel_wait(cpu_dmac_channel_t ch)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        /* Don't wait if DMAC is disabled */
        if ((cpu_ioregs->dmaor & 0x00000001) == 0x00000000) {
                return;
        }

        switch (ch) {
        case 0:
                /* Don't wait if the channel isn't enabled */
                if ((cpu_ioregs->chcr0 & 0x00000001) == 0x00000000) {
                        return;
                }

                /* TE bit will always be set upon normal or abnormal transfer */
                while ((cpu_ioregs->chcr0 & 0x00000002) == 0x00000000) {
                }
                break;
        case 1:
                /* Don't wait if the channel isn't enabled */
                if ((cpu_ioregs->chcr1 & 0x00000001) == 0x00000000) {
                        return;
                }

                /* TE bit will always be set upon normal or abnormal transfer */
                while ((cpu_ioregs->chcr1 & 0x00000002) == 0x00000000) {
                }
                break;
        }
}

void
cpu_dmac_transfer(cpu_dmac_channel_t ch, void *dst, const void *src, size_t size)
{
        cpu_dmac_cfg_t dmac_cfg = {
                .src_mode = CPU_DMAC_SOURCE_INCREMENT,
                .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
                .stride   = CPU_DMAC_STRIDE_4_BYTES,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .ihr      = NULL,
                .ihr_work = NULL
        };

        dmac_cfg.channel = ch;
        dmac_cfg.src     = (uint32_t)src;
        dmac_cfg.dst     = CPU_CACHE_THROUGH | (uintptr_t)dst;
        dmac_cfg.len     = size;

        cpu_dmac_channel_wait(ch);
        cpu_dmac_channel_stop(ch);
        cpu_dmac_channel_config_set(&dmac_cfg);
        cpu_dmac_channel_start(ch);
        cpu_dmac_enable();
}

void
cpu_dmac_transfer_wait(cpu_dmac_channel_t ch)
{
        cpu_dmac_channel_wait(ch);
}

void
cpu_dmac_memset(cpu_dmac_channel_t ch, void *dst, uint32_t value,
    size_t size)
{
        static cpu_dmac_cfg_t dmac_cfg = {
                .src_mode = CPU_DMAC_SOURCE_FIXED,
                .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
                .stride   = CPU_DMAC_STRIDE_4_BYTES,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .ihr      = NULL,
                .ihr_work = NULL
        };

        static volatile uint32_t clear_value __uncached = 0;

        dmac_cfg.channel = ch;
        dmac_cfg.src = (uint32_t)&clear_value;
        dmac_cfg.dst = CPU_CACHE_THROUGH | (uintptr_t)dst;
        dmac_cfg.len = size;

        cpu_dmac_channel_wait(ch);

        clear_value = value;

        cpu_dmac_channel_stop(ch);
        cpu_dmac_channel_config_set(&dmac_cfg);
        cpu_dmac_channel_start(ch);
        cpu_dmac_enable();
        cpu_dmac_channel_wait(ch);
}

static void __interrupt_handler
_master_ch0_ihr_handler(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        callback_call(&_master_ihr_callbacks[IHR_INDEX_CH0]);

        cpu_ioregs->chcr0 &= ~0x00000005;
}

static void __interrupt_handler
_slave_ch0_ihr_handler(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        callback_call(&_slave_ihr_callbacks[IHR_INDEX_CH0]);

        cpu_ioregs->chcr0 &= ~0x00000005;
}

static void __interrupt_handler
_master_ch1_ihr_handler(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        callback_call(&_master_ihr_callbacks[IHR_INDEX_CH1]);

        cpu_ioregs->chcr1 &= ~0x00000005;
}

static void __interrupt_handler
_slave_ch1_ihr_handler(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        callback_call(&_slave_ihr_callbacks[IHR_INDEX_CH1]);

        cpu_ioregs->chcr1 &= ~0x00000005;
}

static callback_t *
_ihr_callback_get(void)
{
        const cpu_which_t which_cpu = cpu_dual_executor_get();

        return _ihr_callback_tables[which_cpu];
}
