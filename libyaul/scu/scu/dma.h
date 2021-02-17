/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_DMA_H_
#define _SCU_DMA_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <scu/map.h>
#include <scu/ic.h>

__BEGIN_DECLS

/// @defgroup SCU_DMA SCU DMA
/// SCU-DMA should be activated by the Master CPU.
///
/// -# SCU-DMA is for transfers between different buses:
///   - HWRAM ↔ A-bus
///   - HWRAM ↔ B-bus
///   - A-bus ↔ B-bus
/// -# Writing to the A-Bus is prohibited.
/// -# Read of the VDP2 is prohibited.
/// -# Reading from or writing to LWRAM locks up the machine.
/// -# Restriction on write address addition value in DMA based on access
///    address:
///   - HWRAM                            → 0x02 can be set
///   - External areas 1 to 3            → 0x02 can be set
///   - External area 4 (A-bus I/O area) → 0x00 and 0x02 can be set
///   - VDP1, VDP2, SCSP                 → all values can be set

/// @addtogroup SCU_DMA
/// @{

/// @brief Not yet documented.
#define SCU_DMA_LEVEL_COUNT 3

/// @brief SCU-DMA transfer mode.
typedef enum scu_dma_mode {
        /// @brief Not yet documented.
        SCU_DMA_MODE_DIRECT   = 0x00,
        /// @brief Not yet documented.
        SCU_DMA_MODE_INDIRECT = 0x01
} scu_dma_mode_t;

/// @brief SCU-DMA starting factors.
typedef enum scu_dma_start_factor {
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_VBLANK_IN       = 0x00,
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_VBLANK_OUT      = 0x01,
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_HBLANK_IN       = 0x02,
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_TIMER_0         = 0x03,
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_TIMER_1         = 0x04,
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_SOUND_REQ       = 0x05,
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_SPRITE_DRAW_END = 0x06,
        /// @brief Not yet documented.
        SCU_DMA_START_FACTOR_ENABLE          = 0x07
} scu_dma_start_factor_t;

/// @brief SCU-DMA transfer stride.
typedef enum scu_dma_stride {
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_0_BYTES   = 0x00,
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_2_BYTES   = 0x01,
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_4_BYTES   = 0x02,
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_8_BYTES   = 0x03,
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_16_BYTES  = 0x04,
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_32_BYTES  = 0x05,
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_64_BYTES  = 0x06,
        /// @brief Not yet documented.
        SCU_DMA_STRIDE_128_BYTES = 0x07
} scu_dma_stride_t;

/// @brief SCU-DMA transfer update type.
typedef enum scu_dma_update {
        /// @brief Not yet documented.
        SCU_DMA_UPDATE_NONE = 0x00000000UL,
        /// @brief Not yet documented.
        SCU_DMA_UPDATE_RUP  = 0x00010000UL,
        /// @brief Not yet documented.
        SCU_DMA_UPDATE_WUP  = 0x00000100UL
} scu_dma_update_t;

/// @brief Not yet documented.
#define SCU_DMA_INDIRECT_TABLE_END (0x80000000UL)

typedef enum scu_dma_bus {
        /// @brief Not yet documented.
        SCU_DMA_BUS_NONE = 0x00,
        /// @brief Not yet documented.
        SCU_DMA_BUS_A    = 0x01,
        /// @brief Not yet documented.
        SCU_DMA_BUS_B    = 0x02,
        /// @brief Not yet documented.
        SCU_DMA_BUS_DSP  = 0x04        
} scu_dma_bus_t;

/// @brief Not yet documented.
#define SCU_DMA_MODE_XFER_INITIALIZER(_len, _dst, _src)                        \
{                                                                              \
        .len = (_len),                                                         \
        .dst = (uint32_t)(_dst),                                               \
        .src = (uint32_t)(_src)                                                \
}

/// @brief Not yet documented.
#define SCU_DMA_MODE_XFER_END_INITIALIZER(_len, _dst, _src)                    \
{                                                                              \
        .len = _len,                                                           \
        .dst = (uint32_t)(_dst),                                               \
        .src = SCU_DMA_INDIRECT_TABLE_END | (uint32_t)(_src)                   \
}

/// Not yet documented.
typedef uint8_t scu_dma_level_t;

/// @brief Not yet documented.
typedef struct scu_dma_handle {
        /// Not yet documented.
        uint32_t dnr;
        /// Not yet documented.
        uint32_t dnw;
        /// Not yet documented.
        uint32_t dnc;
        /// Not yet documented.
        uint32_t dnad;
        /// Not yet documented.
        uint32_t dnmd;
} __packed __aligned(4) scu_dma_handle_t;

/// @brief Not yet documented.
typedef struct scu_dma_xfer {
        /// Not yet documented.
        uint32_t len;
        /// Not yet documented.
        uint32_t dst;
        /// Not yet documented.
        uint32_t src;
} __packed __aligned(4) scu_dma_xfer_t;

/// @brief Not yet documented.
typedef struct scu_dma_level_cfg {
        /// Not yet documented.
        scu_dma_mode_t mode:8;

        /// Not yet documented.
        union {
                /// Indirect mode
                scu_dma_xfer_t *indirect;

                /// Direct mode
                scu_dma_xfer_t direct;
        } xfer;

        /// Not yet documented.
        scu_dma_stride_t stride:8;
        /// Not yet documented.
        scu_dma_update_t update;
} scu_dma_level_cfg_t;

/// @brief Not yet documented.
typedef void (*scu_dma_ihr_t)(void);

/// @brief Not yet documented.
typedef void (*scu_dma_callback_t)(void *);

/// @brief Not yet documented.
extern void scu_dma_level_stop(scu_dma_level_t level);

/// @brief Not yet documented.
static inline uint32_t __always_inline
scu_dma_status_get(void)
{
        return MEMORY_READ(32, SCU(DSTA));
}

/// @brief Not yet documented.
static inline uint32_t __always_inline
scu_dma_dsp_busy(void)
{
        /* In operation, on standby, or interrupted to background */
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00010003);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_dsp_wait(void)
{
        while ((scu_dma_dsp_busy()) != 0x00000000);
}

/// @brief Not yet documented.
static inline scu_dma_bus_t __always_inline
scu_dma_bus_access_busy(void)
{
        return ((MEMORY_READ(32, SCU(DSTA)) >> 20) & 0x07);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_bus_access_wait(scu_dma_bus_t mask)
{
        while (((scu_dma_bus_access_busy()) & mask) != 0x00);
}

/// @brief Not yet documented.
static inline uint32_t __always_inline
scu_dma_level0_busy(void)
{
        /* In operation, on standby, or interrupted */
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00010030);
}

/// @brief Not yet documented.
static inline uint32_t __always_inline
scu_dma_level1_busy(void)
{
        /* In operation, on standby, or interrupted */
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00020300);
}

/// @brief Not yet documented.
static inline uint32_t __always_inline
scu_dma_level2_busy(void)
{
        /* In operation, on standby, or interrupted */
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00003000);
}

/// @brief Not yet documented.
static inline uint32_t __always_inline
scu_dma_level_busy(scu_dma_level_t level)
{
        switch (level & SCU_DMA_LEVEL_COUNT) {
        case 1:
                return scu_dma_level1_busy();
        case 2:
                return scu_dma_level2_busy();
        case 0:
        default:
                return scu_dma_level0_busy();
        }
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_stop(void)
{
        scu_dma_level_stop(0);
        scu_dma_level_stop(1);
        scu_dma_level_stop(2);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_illegal_set(scu_dma_ihr_t ihr)
{
        scu_ic_ihr_set(SCU_IC_INTERRUPT_DMA_ILLEGAL, ihr);
}

/// @brief Not yet documented.
extern void scu_dma_config_buffer(scu_dma_handle_t *handle,
    const scu_dma_level_cfg_t *cfg);

/// @brief Not yet documented.
extern void scu_dma_config_set(scu_dma_level_t level,
    scu_dma_start_factor_t start_factor, const scu_dma_handle_t *handle,
    scu_dma_callback_t callback);

/// @ingroup SCU_IC_HELPERS
/// @brief Not yet documented.
extern void scu_dma_level_end_set(scu_dma_level_t level,
    scu_dma_callback_t callback, void *work);

/// @brief Not yet documented.
extern void scu_dma_level_fast_start(scu_dma_level_t level);

/// @brief Not yet documented.
extern void scu_dma_level_start(scu_dma_level_t level);

/// @brief Not yet documented.
extern void scu_dma_level_wait(scu_dma_level_t level);

/// @brief Not yet documented.
extern scu_dma_level_t scu_dma_level_unused_get(void);

/// @}

__END_DECLS

#endif /* !_SCU_DMA_H_ */
