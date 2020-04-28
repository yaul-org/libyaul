/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_DMAC_H_
#define _CPU_DMAC_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>

#include <cpu/map.h>

__BEGIN_DECLS

#define CPU_DMAC_PRIORITY_MODE_FIXED            0x00
#define CPU_DMAC_PRIORITY_MODE_ROUND_ROBIN      0x01

typedef void (*cpu_dmac_ihr)(void *);

typedef struct cpu_dmac_cfg {
        uint8_t channel;

#define CPU_DMAC_DESTINATION_FIXED      0x00
#define CPU_DMAC_DESTINATION_INCREMENT  0x01
#define CPU_DMAC_DESTINATION_DECREMENT  0x02
        uint8_t src_mode;

#define CPU_DMAC_SOURCE_FIXED           0x00
#define CPU_DMAC_SOURCE_INCREMENT       0x01
#define CPU_DMAC_SOURCE_DECREMENT       0x02
        uint8_t dst_mode;

#define CPU_DMAC_STRIDE_1_BYTE          0x00
#define CPU_DMAC_STRIDE_2_BYTES         0x01
#define CPU_DMAC_STRIDE_4_BYTES         0x02
#define CPU_DMAC_STRIDE_16_BYTES        0x03
        uint8_t stride;

#define CPU_DMAC_BUS_MODE_CYCLE_STEAL   0x00
#define CPU_DMAC_BUS_MODE_BURST         0x01
        uint8_t bus_mode;

        uint32_t src;
        uint32_t dst;
        uint32_t len;

        cpu_dmac_ihr ihr;
        void *ihr_work;
} cpu_dmac_cfg_t;

typedef struct cpu_dmac_status {
        unsigned int enabled:1;
        unsigned int priority_mode:1;
        unsigned int channel_enabled:2;
        unsigned int channel_busy:2;
        unsigned int address_error:1;
        unsigned int nmi_interrupt:1;
} __packed cpu_dmac_status_t;

static inline void __always_inline
cpu_dmac_channel_transfer_set(uint8_t ch, uint32_t tcr_bits)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        MEMORY_WRITE(32, CPU(TCR0 | n), tcr_bits);
}

static inline void __always_inline
cpu_dmac_enable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x0000000F);
        MEMORY_WRITE_OR(32, CPU(DMAOR), 0x00000001);
}

static inline void __always_inline
cpu_dmac_disable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
}

static inline uint8_t __always_inline
cpu_dmac_interrupt_priority_get(void)
{
        uint16_t ipra;
        ipra = MEMORY_READ(16, CPU(IPRA));

        return ((ipra >> 8) & 0x0F);
}

static inline void __always_inline
cpu_dmac_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), 0xF7FF);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 8);
}

static inline void __always_inline
cpu_dmac_priority_mode_set(uint8_t mode)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
        MEMORY_WRITE_OR(32, CPU(DMAOR), (mode & 0x01) << 3);
}

static inline void __always_inline
cpu_dmac_channel_start(uint8_t ch)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000003);
        MEMORY_WRITE_OR(32, CPU(CHCR0 | n), 0x00000001);
}

static inline void __always_inline
cpu_dmac_channel_stop(uint8_t ch)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        /* Don't clear the status bits */
        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000001);
}

static inline void __always_inline
cpu_dmac_stop(void)
{
        cpu_dmac_channel_stop(0);
        cpu_dmac_channel_stop(1);
}

extern void cpu_dmac_status_get(cpu_dmac_status_t *);
extern void cpu_dmac_channel_config_set(const cpu_dmac_cfg_t *);
extern void cpu_dmac_channel_wait(uint8_t);

__END_DECLS

#endif /* !_CPU_DMAC_H */
