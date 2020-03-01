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

/// @defgroup CPU_DMAC_DEFINES
/// @defgroup CPU_DMAC_STRUCTURES
/// @defgroup CPU_DMAC_INLINE_FUNCTIONS
/// @defgroup CPU_DMAC_FUNCTIONS

/// @addtogroup CPU_DMAC_DEFINES
/// @{

/// Not yet documented.
#define CPU_DMAC_PRIORITY_MODE_FIXED            0x00
/// Not yet documented.
#define CPU_DMAC_PRIORITY_MODE_ROUND_ROBIN      0x01

/// Not yet documented.
#define CPU_DMAC_DESTINATION_FIXED      0x00
/// Not yet documented.
#define CPU_DMAC_DESTINATION_INCREMENT  0x01
/// Not yet documented.
#define CPU_DMAC_DESTINATION_DECREMENT  0x02

/// Not yet documented.
#define CPU_DMAC_SOURCE_FIXED           0x00
/// Not yet documented.
#define CPU_DMAC_SOURCE_INCREMENT       0x01
/// Not yet documented.
#define CPU_DMAC_SOURCE_DECREMENT       0x02

/// Not yet documented.
#define CPU_DMAC_STRIDE_1_BYTE          0x00
/// Not yet documented.
#define CPU_DMAC_STRIDE_2_BYTES         0x01
/// Not yet documented.
#define CPU_DMAC_STRIDE_4_BYTES         0x02
/// Not yet documented.
#define CPU_DMAC_STRIDE_16_BYTES        0x03

/// Not yet documented.
#define CPU_DMAC_BUS_MODE_CYCLE_STEAL   0x00
/// Not yet documented.
#define CPU_DMAC_BUS_MODE_BURST         0x01

/// @}

/// @addtogroup CPU_DMAC_STRUCTURES
/// @{

/// @compound
/// @brief Not yet documented.
struct cpu_dmac_cfg {
        /// Not yet documented.
        uint8_t channel;
        /// Not yet documented.
        uint8_t src_mode;
        /// Not yet documented.
        uint8_t dst_mode;
        /// Not yet documented.
        uint8_t stride;
        /// Not yet documented.
        uint8_t bus_mode;

        /// Not yet documented.
        uint32_t src;
        /// Not yet documented.
        uint32_t dst;
        /// Not yet documented.
        uint32_t len;

        /// Not yet documented.
        void (*ihr)(void);
};

/// @compound
/// @brief Not yet documented.
struct dmac_status {
        /// Not yet documented.
        unsigned int enabled:1;
        /// Not yet documented.
        unsigned int priority_mode:1;
        /// Not yet documented.
        unsigned int channel_enabled:2;
        /// Not yet documented.
        unsigned int channel_busy:2;
        /// Not yet documented.
        unsigned int address_error:1;
        /// Not yet documented.
        unsigned int nmi_interrupt:1;
} __packed;

/// @}

/// @addtogroup CPU_DMAC_INLINE_FUNCTIONS
/// @{

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_channel_transfer_set(uint8_t ch, uint32_t tcr_bits)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        MEMORY_WRITE(32, CPU(TCR0 | n), tcr_bits);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_enable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x0000000F);
        MEMORY_WRITE_OR(32, CPU(DMAOR), 0x00000001);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_disable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), ~0x1F00);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 8);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_priority_mode_set(uint8_t mode)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
        MEMORY_WRITE_OR(32, CPU(DMAOR), (mode & 0x01) << 3);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_channel_start(uint8_t ch)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000003);
        MEMORY_WRITE_OR(32, CPU(CHCR0 | n), 0x00000001);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_channel_stop(uint8_t ch)
{
        uint32_t n;
        n = (ch & 0x01) << 4;

        /* Don't clear the status bits */
        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000001);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dmac_stop(void)
{
        cpu_dmac_channel_stop(0);
        cpu_dmac_channel_stop(1);
}

/// @}

/// @addtogroup CPU_DMAC_INLINE_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void cpu_dmac_init(void);

/// @brief Not yet documented.
extern void cpu_dmac_status_get(struct dmac_status *);

/// @brief Not yet documented.
extern void cpu_dmac_channel_config_set(const struct cpu_dmac_cfg *);

/// @brief Not yet documented.
extern void cpu_dmac_channel_wait(uint8_t);

/// @}

__END_DECLS

#endif /* !_CPU_DMAC_H */
