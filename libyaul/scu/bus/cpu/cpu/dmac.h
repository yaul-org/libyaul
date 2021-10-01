/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_DMAC_H_
#define _YAUL_CPU_DMAC_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include <cpu/map.h>

__BEGIN_DECLS

/// @defgroup CPU_DMAC CPU Direct Memory Access Controller (DMAC)

/// @addtogroup CPU_DMAC
/// @{

/// @brief CPU-DMAC priority mode.
/// @see cpu_dmac_priority_mode_set
typedef enum cpu_dmac_priority_mode {
        /// @brief Fixed priority mode.
        /// @details Channel #0 has higher priority over channel #1.
        CPU_DMAC_PRIORITY_MODE_FIXED       = 0x00,
        /// @brief Round-robin mode.
        CPU_DMAC_PRIORITY_MODE_ROUND_ROBIN = 0x01
} cpu_dmac_priority_mode_t;

/// @brief Transfer source address modes.
/// @see cpu_dmac_cfg_t
typedef enum cpu_dmac_src {
        /// @brief Fixed source address.
        CPU_DMAC_SOURCE_FIXED     = 0x00,
        /// @brief Source address is incremented.
        CPU_DMAC_SOURCE_INCREMENT = 0x01,
        /// @brief Source address is decremented.
        CPU_DMAC_SOURCE_DECREMENT = 0x02
} cpu_dmac_src_t;

/// @brief Transfer destination type.
/// @see cpu_dmac_cfg_t
typedef enum cpu_dmac_dst {
        /// @brief Fixed destination address.
        CPU_DMAC_DESTINATION_FIXED     = 0x00,
        /// @brief Destination address is incremented.
        CPU_DMAC_DESTINATION_INCREMENT = 0x01,
        /// @brief Destination address is decremented.
        CPU_DMAC_DESTINATION_DECREMENT = 0x02
} cpu_dmac_dst_t;

/// @brief Transfer stride bytes.
/// @see cpu_dmac_cfg_t
typedef enum cpu_dmac_stride {
        /// @brief 1-byte stride.
        CPU_DMAC_STRIDE_1_BYTE   = 0x00,
        /// @brief 2-byte stride.
        CPU_DMAC_STRIDE_2_BYTES  = 0x01,
        /// @brief 4-byte stride.
        CPU_DMAC_STRIDE_4_BYTES  = 0x02,
        /// @brief 16-byte stride.
        CPU_DMAC_STRIDE_16_BYTES = 0x03
} cpu_dmac_stride_t;

/// @brief Transfer bus mode.
/// @see cpu_dmac_cfg_t
typedef enum cpu_dmac_bus_mode {
        /// @brief Select bus mode to cycle-steal mode.
        CPU_DMAC_BUS_MODE_CYCLE_STEAL = 0x00,
        /// @brief Select bus mode to burst mode.
        CPU_DMAC_BUS_MODE_BURST       = 0x01
} cpu_dmac_bus_mode_t;

/// @brief Callback type.
/// @see cpu_dmac_cfg_t.ihr
/// @see cpu_dmac_cfg_t.ihr_work
typedef void (*cpu_dmac_ihr)(void *);

/// CPU-DMAC channel.
typedef uint8_t cpu_dmac_channel_t;

/// @brief CPU-DMAC configuration.
typedef struct cpu_dmac_cfg {
        /// Channel
        cpu_dmac_channel_t channel:2;

        /// Source mode
        cpu_dmac_src_t src_mode:2;

        /// Destination mode.
        cpu_dmac_dst_t dst_mode:2;

        /// Stride.
        cpu_dmac_stride_t stride:3;

        /// Bus mode.
        cpu_dmac_bus_mode_t bus_mode:1;

        /// Memory transfer source address.
        uint32_t src;

        /// Memory transfer destination address.
        uint32_t dst;

        /// Transfer length.
        uint32_t len;

        /// @ingroup CPU_INTC_HELPERS
        /// @brief Callback when transfer is completed.
        ///
        /// @details Set to `NULL` if no callback is desired.
        cpu_dmac_ihr ihr;

        /// @ingroup CPU_INTC_HELPERS
        /// @brief Pointer to any work passed onto @ref cpu_dmac_cfg_t.ihr.
        ///
        /// @details If @ref cpu_dmac_cfg_t.ihr is `NULL`, @ref
        /// cpu_dmac_cfg_t.ihr_work is ignored.
        void *ihr_work;
} __aligned(4) cpu_dmac_cfg_t;

static_assert(sizeof(cpu_dmac_cfg_t) == 24);

/// @brief CPU-DMAC status.
typedef struct cpu_dmac_status {
        /// Flag to determine if CPU-DMAC transfers are enabled on all channels.
        bool enabled:1;
        /// Priority mode.
        cpu_dmac_priority_mode_t priority_mode:1;
        /// Bit field to determine which channel(s) are enabled.
        unsigned int channel_enabled:2;
        /// Bit field to determine which channel(s) are busy.
        unsigned int channel_busy:2;
        /// Flag to determine if an address error has occurred.
        bool address_error:1;
        /// Flag to determine if NMI interrupt has occurred.
        bool nmi_interrupt:1;
} __packed cpu_dmac_status_t;

/// @brief Write directly to the CPU-DMAC I/O @ref TCR0 or @ref TCR1 register.
///
/// @details This function is not enough to set up a proper transfer. Use @ref
/// cpu_dmac_channel_config_set to configure a transfer. The purpose of this
/// function is to quickly reset a transfer by just setting the transfer length.
///
/// There is no waiting if the CPU-DMAC or the specific channel @p ch is
/// operating.
/// 
/// While this function resets the transfer length, it does not start the
/// transfer again. For that, use @ref cpu_dmac_channel_start.
/// 
/// @param ch       The channel.
/// @param tcr_bits The 4-byte I/O register value.
///
/// @see cpu_dmac_channel_config_set
/// @see cpu_dmac_channel_start
static inline void __always_inline
cpu_dmac_channel_transfer_set(cpu_dmac_channel_t ch, uint32_t tcr_bits)
{
        const uint32_t n = (ch & 0x01) << 4;

        MEMORY_WRITE(32, CPU(TCR0 | n), tcr_bits);
}

/// @brief Enable CPU-DMAC.
static inline void __always_inline
cpu_dmac_enable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x0000000F);
        MEMORY_WRITE_OR(32, CPU(DMAOR), 0x00000001);
}

/// @brief Disable CPU-DMAC.
static inline void __always_inline
cpu_dmac_disable(void)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
}

/// @brief Obtain the interrupt priority level for CPU-DMAC.
/// @returns The interrupt priority level ranging from `0` to `15`.
static inline uint8_t __always_inline
cpu_dmac_interrupt_priority_get(void)
{
        const uint16_t ipra = MEMORY_READ(16, CPU(IPRA));

        return ((ipra >> 8) & 0x0F);
}

/// @brief Set the interrupt priority level for CPU-DMAC.
///
/// @param priority The priority ranging from `0` to `15`.
static inline void __always_inline
cpu_dmac_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), 0xF7FF);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 8);
}

/// @brief Set the priority mode.
///
/// @param mode The priority mode.
static inline void __always_inline
cpu_dmac_priority_mode_set(cpu_dmac_priority_mode_t mode)
{
        MEMORY_WRITE_AND(32, CPU(DMAOR), ~0x00000001);
        MEMORY_WRITE_OR(32, CPU(DMAOR), (mode & 0x01) << 3);
}

/// @brief Start the CPU-DMAC channel transfer.
///
/// @details There is no check if the CPU-DMAC channel @p ch is currently
/// operating.
///
/// There is no check if the CPU-DMAC channel @p ch has been configured.
static inline void __always_inline
cpu_dmac_channel_start(cpu_dmac_channel_t ch)
{
        const uint32_t n = (ch & 0x01) << 4;

        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000003);
        MEMORY_WRITE_OR(32, CPU(CHCR0 | n), 0x00000001);
}

/// @brief Stop specific CPU-DMAC channel transfer.
///
/// @param ch The channel.
static inline void __always_inline
cpu_dmac_channel_stop(cpu_dmac_channel_t ch)
{
        const uint32_t n = (ch & 0x01) << 4;

        /* Don't clear the status bits */
        MEMORY_WRITE_AND(32, CPU(CHCR0 | n), ~0x00000001);
}

/// @brief Stop all CPU-DMAC channel transfers (if any).
static inline void __always_inline
cpu_dmac_stop(void)
{
        cpu_dmac_channel_stop(0);
        cpu_dmac_channel_stop(1);
}

/// @brief Obtain CPU-DMAC operation status.
///
/// @details If @p status is `NULL`, the status will not be updated.
///
/// @param[out] status The pointer to @ref cpu_dmac_status_t
extern void cpu_dmac_status_get(cpu_dmac_status_t *status);

/// @brief Configure a CPU-DMAC channel for transfer.
///
/// @details Configuring the CPU-DMAC channel in @p cfg does start the transfer.
/// To start the transfer, use @ref cpu_dmac_channel_start.
///
/// The CPU-DMAC channel is forcefully stopped upon starting the configuration.
/// If the channel is currently operating, use @ref cpu_dmac_channel_wait to
/// wait until the transfer is complete.
/// 
/// @param[in] cfg The CPU-DMAC transfer configuration.
///
/// @see cpu_dmac_channel_start
/// @see cpu_dmac_channel_stop
/// @see cpu_dmac_channel_wait
/// @see cpu_dmac_channel_transfer_set
extern void cpu_dmac_channel_config_set(const cpu_dmac_cfg_t *cfg);

/// @brief Busy wait for transfer completion of CPU-DMAC channel.
///
/// @details There is no waiting if the CPU-DMAC or the specific channel @p ch
/// is disabled.
///
/// @param ch The channel.
extern void cpu_dmac_channel_wait(cpu_dmac_channel_t ch);

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_DMAC_H_ */
