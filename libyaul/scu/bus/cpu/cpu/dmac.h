/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_DMAC_H_
#define _CPU_DMAC_H_

#include <stdint.h>
#include <stddef.h>

#include <cpu/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct dmac_ch_cfg {
        uint8_t ccc_ch;

#define CPU_DMAC_DESTINATION_FIXED      0x00
#define CPU_DMAC_DESTINATION_INCREMENT  0x01
#define CPU_DMAC_DESTINATION_DECREMENT  0x02
        uint8_t ccc_src_mode;

#define CPU_DMAC_SOURCE_FIXED           0x00
#define CPU_DMAC_SOURCE_INCREMENT       0x01
#define CPU_DMAC_SOURCE_DECREMENT       0x02
        uint8_t ccc_dst_mode;

#define CPU_DMAC_STRIDE_1_BYTE          0x00
#define CPU_DMAC_STRIDE_2_BYTES         0x01
#define CPU_DMAC_STRIDE_4_BYTES         0x02
#define CPU_DMAC_STRIDE_16_BYTES        0x03
        uint8_t ccc_stride;

        uint32_t ccc_src;
        uint32_t ccc_dst;
        uint32_t ccc_len;

        void (*ccc_ihr)(void);
};

struct dmac_ch_status {
        uint32_t _;
};

static inline void __attribute__ ((always_inline))
cpu_dmac_enable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x0000000F);
        MEMORY_WRITE_OR(32, CPU(DMAOR), 0x00000001);
}

static inline void __attribute__ ((always_inline))
cpu_dmac_disable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
}

static inline void __attribute__ ((always_inline))
cpu_dmac_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), ~0x1F00);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 8);
}

static inline void __attribute__ ((always_inline))
cpu_dmac_priority_mode_set(uint8_t mode)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
        MEMORY_WRITE_OR(32, CPU(DMAOR), (mode & 0x01) << 3);
}

static inline void __attribute__ ((always_inline))
cpu_dmac_channel_start(uint8_t ch)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000003);
        MEMORY_WRITE_OR(32, CPU(CHCR0 | n), 0x00000001);
}

static inline void __attribute__ ((always_inline))
cpu_dmac_channel_stop(uint8_t ch)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        /* Don't clear the status bits */
        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000001);
}

extern void cpu_dmac_init(void);
extern void cpu_dmac_channel_config_set(const struct dmac_ch_cfg *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CPU_DMAC_H */
