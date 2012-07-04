/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/dmac.h>
#include <cpu/intc.h>
#include <scu/ic.h>

#include <assert.h>

#include "cpu-internal.h"

/* IHR */
static void (*ihr)(void);

static void trampoline(void);

void
cpu_dmac_channel_set(struct cpu_channel_cfg *cfg)
{
        uint32_t len;
        uint32_t chcr;
        uint32_t vcrdma;
        uint32_t ipra;

        assert((cfg->ch == CPU_DMAC_CHANNEL(0)) ||
            (cfg->ch == CPU_DMAC_CHANNEL(1)));

        /* Source and destination address modes */
        chcr = cfg->src.mode | cfg->dst.mode;

        /* Able to transfer at most 16MiB exclusive when TCR0 is
         * 0x00FFFFFF, 16MliB inclusive when TCR0 is 0x00000000 */
        len = cfg->len;
        switch (cfg->xfer_size) {
        case 1:
                /* Byte unit */
                break;
        case 2:
                /* Word (2-byte) unit */
                chcr |= 0x00000400;
                len >>= 1;
                break;
        case 4:
                /* Long (4-byte) unit */
                chcr |= 0x00000800;
                len >>= 2;
                break;
        case 16:
                /* 16-byte unit */
                chcr |= 0x00000C00;
                /* For 16-byte transfers, set the count to 4
                 * times the number of transfers */
                len <<= 2;
                break;
        default:
                assert((cfg->xfer_size == 1) ||
                    (cfg->xfer_size == 2) ||
                    (cfg->xfer_size == 16));
                return;
        }

        assert(cfg->len <= 0x01000000);
        len &= 0x00FFFFFF;

        vcrdma = 0x00000000;
        if (cfg->ihr != NULL) {
                assert(cfg->vector <= 0x7F);

                vcrdma = cfg->vector;
                /* Interrupt enable */
                chcr |= 0x00000004;
                /* Set interrupt handling routine */
                ihr = cfg->ihr;
                cpu_intc_interrupt_set(cfg->vector, trampoline);

                /* Set priority */
                assert(cfg->priority <= 15);

                ipra = MEMORY_READ(16, CPU(IPRA));
                ipra &= 0x00FF;
                ipra |= cfg->priority << 8;

                /* Write to memory */
                MEMORY_WRITE(16, CPU(IPRA), ipra);
        }

        switch (cfg->ch) {
        case CPU_DMAC_CHANNEL(0):
                MEMORY_WRITE(32, CPU(DAR0), (uint32_t)cfg->dst.ptr);
                MEMORY_WRITE(32, CPU(SAR0), (uint32_t)cfg->src.ptr);
                MEMORY_WRITE(32, CPU(TCR0), len);
                MEMORY_WRITE(8, CPU(DRCR0), 0x00);
                MEMORY_WRITE(32, CPU(VCRDMA0), vcrdma);
                MEMORY_WRITE(32, CPU(CHCR0), chcr);
                return;
        case CPU_DMAC_CHANNEL(1):
                MEMORY_WRITE(32, CPU(DAR1), (uint32_t)cfg->dst.ptr);
                MEMORY_WRITE(32, CPU(SAR1), (uint32_t)cfg->src.ptr);
                MEMORY_WRITE(32, CPU(TCR1), len);
                MEMORY_WRITE(8, CPU(DRCR1), 0x00);
                MEMORY_WRITE(32, CPU(VCRDMA1), vcrdma);
                MEMORY_WRITE(32, CPU(CHCR1), chcr);
                return;
        }
}

static void
trampoline(void)
{
        uint16_t ipra;

        ipra = MEMORY_READ(16, CPU(IPRA));
        ipra &= 0x00FF;
        MEMORY_WRITE(16, CPU(IPRA), ipra);

        /* Call user IHR */
        ihr();
}
