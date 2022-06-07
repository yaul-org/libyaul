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

static void _dmac_ch0_ihr_handler(void);
static void _dmac_ch1_ihr_handler(void);

#define CPU_DMAC_IHR_INDEX_CH0 0
#define CPU_DMAC_IHR_INDEX_CH1 1

static callback_t _master_ihr_callbacks[2];
static callback_t _slave_ihr_callbacks[2];

static callback_t *_dmac_executor_ihr_callbacks_get(void);

void
__cpu_dmac_init(void)
{
        cpu_dmac_channel_wait(0);
        cpu_dmac_channel_wait(1);
        cpu_dmac_channel_stop(0);
        cpu_dmac_channel_stop(1);
        cpu_dmac_disable();

        MEMORY_WRITE(32, CPU(VCRDMA0), CPU_INTC_INTERRUPT_DMAC0);
        MEMORY_WRITE(32, CPU(VCRDMA1), CPU_INTC_INTERRUPT_DMAC1);

        cpu_dmac_interrupt_priority_set(15);

        MEMORY_WRITE(8, CPU(DRCR0), 0x00);
        MEMORY_WRITE(8, CPU(DRCR1), 0x00);

        const uint8_t which_cpu = cpu_dual_executor_get();

        if (which_cpu == CPU_MASTER) {
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC0, _dmac_ch0_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC1, _dmac_ch1_ihr_handler);

                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC0 + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _dmac_ch0_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMAC1 + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _dmac_ch1_ihr_handler);
        }

        callback_init(&_master_ihr_callbacks[0]);
        callback_init(&_master_ihr_callbacks[1]);

        callback_init(&_slave_ihr_callbacks[0]);
        callback_init(&_slave_ihr_callbacks[1]);

        cpu_dmac_enable();
}

void
cpu_dmac_status_get(cpu_dmac_status_t *status)
{
        if (status == NULL) {
                return;
        }

        const uint32_t reg_dmaor = MEMORY_READ(32, CPU(DMAOR));

        status->enabled = reg_dmaor & 0x00000001;
        status->priority_mode = (reg_dmaor >> 3) & 0x00000001;
        status->address_error = (reg_dmaor >> 2) & 0x00000001;
        status->nmi_interrupt = (reg_dmaor >> 1) & 0x00000001;

        const uint32_t reg_chcr0 = MEMORY_READ(32, CPU(CHCR0));
        const uint32_t reg_chcr1 = MEMORY_READ(32, CPU(CHCR1));

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
        const uint32_t n = (cfg->channel & 0x01) << 4;

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

        /* Check that the source and destination addresses are stride-byte
         * aligned */

        if ((stride == CPU_DMAC_STRIDE_16_BYTES) || (reg_tcr > 0x00FFFFFF)) {
                stride = CPU_DMAC_STRIDE_16_BYTES;

                /* Transfer 16MiB inclusive when TCR0 is 0x00000000 */
                reg_tcr = 0x00000000;

                /* During 16-byte transfers, the transfer address mode bit for
                 * dual address mode */
                reg_chcr &= ~0x00000008;
        } else {
                if (stride >= CPU_DMAC_STRIDE_4_BYTES) {
                        reg_tcr >>= 2;
                } else if (stride == CPU_DMAC_STRIDE_2_BYTES) {
                        reg_tcr >>= 1;
                }
        }

        if (cfg->ihr != NULL) {
                /* Enable interrupt */
                reg_chcr |= 0x00000004;

                callback_t * const ihr_callbacks =
                    _dmac_executor_ihr_callbacks_get();
                callback_t * const ihr_callback =
                    &ihr_callbacks[cfg->channel];

                /* Set interrupt handling routine */
                callback_set(ihr_callback, cfg->ihr, cfg->ihr_work);
        }

        MEMORY_WRITE(32, CPU(DAR0 | n), (uint32_t)cfg->dst);
        MEMORY_WRITE(32, CPU(SAR0 | n), (uint32_t)cfg->src);
        MEMORY_WRITE(32, CPU(TCR0 | n), reg_tcr);
        MEMORY_WRITE(32, CPU(CHCR0 | n), reg_chcr);
}

void
cpu_dmac_channel_wait(cpu_dmac_channel_t ch)
{
        const uint32_t n = (ch & 0x01) << 4;

        /* Don't wait if DMAC is disabled */
        if ((MEMORY_READ(32, CPU(DMAOR)) & 0x00000001) == 0x00000000) {
                return;
        }

        /* Don't wait if the channel isn't enabled */
        if ((MEMORY_READ(32, CPU(CHCR0 | n)) & 0x00000001) == 0x00000000) {
                return;
        }

        /* TE bit will always be set upon normal or abnormal transfer */
        while ((MEMORY_READ(32, CPU(CHCR0 | n)) & 0x00000002) == 0x00000000) {
        }
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

static callback_t *
_dmac_executor_ihr_callbacks_get(void)
{
        const uint8_t which_cpu = cpu_dual_executor_get();

        switch (which_cpu) {
        case CPU_MASTER:
                return _master_ihr_callbacks;
        case CPU_SLAVE:
                return _slave_ihr_callbacks;
        default:
                return NULL;
        }
}

static void __interrupt_handler
_dmac_ch0_ihr_handler(void)
{
        callback_t * const ihr_callbacks =
            _dmac_executor_ihr_callbacks_get();
        callback_t * const ihr_callback =
            &ihr_callbacks[CPU_DMAC_IHR_INDEX_CH0];

        callback_call(ihr_callback);

        MEMORY_WRITE_AND(32, CPU(CHCR0), ~0x00000005);
}

static void __interrupt_handler
_dmac_ch1_ihr_handler(void)
{
        callback_t * const ihr_callbacks =
            _dmac_executor_ihr_callbacks_get();
        callback_t * const ihr_callback =
            &ihr_callbacks[CPU_DMAC_IHR_INDEX_CH1];

        callback_call(ihr_callback);

        MEMORY_WRITE_AND(32, CPU(CHCR1), ~0x00000005);
}
