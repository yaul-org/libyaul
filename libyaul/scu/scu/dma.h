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

/// @defgroup SCU_DMA_DEFINES
/// @defgroup SCU_DMA_HELPERS
/// @defgroup SCU_DMA_STRUCTURES
/// @defgroup SCU_DMA_INLINE_FUNCTIONS
/// @defgroup SCU_DMA_FUNCTIONS

/*-
 * SCU DMA should be activated by the Master CPU
 *
 * - Keep in mind that:
 *     1. DMA transfers does not begin until it is explictily started
 *     2. DMA transfer level 2 will block the CPU during operation of level 1
 *
 * - SCU DMA is for transfers between different buses:
 *     1. Work RAM-H <-> A-bus
 *     2. Work RAM-H <-> B-bus
 *     3. A-bus <-> B-bus
 *
 * - A-bus write prohibited
 * - VDP2 area read prohibited
 * - WORKRAM-L usage from SCU disabled
 *
 * - Restriction on write address addition value in DMA based on access
 *   address
 *
 *   1. WORKRAM-H                        -> 0x02 can be set
 *   2. External areas 1 to 3            -> 0x02 can be set
 *   3. External area 4 (A-bus I/O area) -> 0x00 and 0x02 can be set
 *   4. VDP1, VDP2, SCSP                 -> all values can be set
 */

/// @addtogroup SCU_DMA_DEFINES
/// @{

/// Transfer direct mode.
/// @see scu_dma_level_cfg::mode
#define SCU_DMA_MODE_DIRECT     0x00
/// Transfer indirect mode.
/// @see scu_dma_level_cfg::mode
#define SCU_DMA_MODE_INDIRECT   0x01

/// Start the transfer on the start of VBLANK-IN.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
#define SCU_DMA_START_FACTOR_VBLANK_IN          0x00
/// Start the transfer on the start of VBLANK-OUT.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
#define SCU_DMA_START_FACTOR_VBLANK_OUT         0x01
/// Start the transfer on the start of HBLANK-IN.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
#define SCU_DMA_START_FACTOR_HBLANK_IN          0x02
/// Start the transfer on the start of SCU-Timer 0.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
#define SCU_DMA_START_FACTOR_TIMER_0            0x03
/// Start the transfer on the start of SCU-Timer 1.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
#define SCU_DMA_START_FACTOR_TIMER_1            0x04
/// Start the transfer on the start of...?
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
#define SCU_DMA_START_FACTOR_SOUND_REQ          0x05
/// Start the transfer on the start of VDP1 sprite draw end.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
#define SCU_DMA_START_FACTOR_SPRITE_DRAW_END    0x06
/// Start the transfer when explicitly started.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set,
///      scu_dma_level0_fast_start,
///      scu_dma_level1_fast_start,
///      scu_dma_level2_fast_start
///      scu_dma_level0_start,
///      scu_dma_level1_start,
///      scu_dma_level2_start,
///      scu_dma_level_start
#define SCU_DMA_START_FACTOR_ENABLE             0x07

/// Transfer stride by 0 bytes.
///
/// Describe it here!
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_0_BYTES          0x00
/// Transfer stride by 2 bytes.
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_2_BYTES          0x01
/// Transfer stride by 4 bytes.
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_4_BYTES          0x02
/// Transfer stride by 8 bytes.
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_8_BYTES          0x03
/// Transfer stride by 16 bytes.
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_16_BYTES         0x04
/// Transfer stride by 32 bytes.
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_32_BYTES         0x05
// Transfer stride by 64 bytes.
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_64_BYTES         0x06
/// Transfer stride by 128 bytes.
/// @see scu_dma_level_cfg::stride
#define SCU_DMA_STRIDE_128_BYTES        0x07

/// Keep the source and destination address fixed.
/// @see scu_dma_level_cfg::update
#define SCU_DMA_UPDATE_NONE     0x00000000
/// Increment the source, but keep the destination address fixed.
/// @see scu_dma_level_cfg::update
#define SCU_DMA_UPDATE_RUP      0x00010000
/// Increment both source and destination addresses.
/// @see scu_dma_level_cfg::update
#define SCU_DMA_UPDATE_WUP      0x00000100

/// Not yet documented.
#define SCU_DMA_BUS_A   0x00
/// Not yet documented.
#define SCU_DMA_BUS_B   0x01
/// Not yet documented.
#define SCU_DMA_BUS_DSP 0x02

/// When SCU-DMA transfer is indirect-mode, the value is bitwise OR'd with the
/// last entry's scu_dma_xfer::len member.
/// @see scu_dma_xfer
/// @warning Not setting this bit on the last entry will lock up the hardware.
#define SCU_DMA_INDIRECT_TBL_END        0x80000000

/// @}

/// @addtogroup SCU_DMA_HELPERS
/// @{

/// Not yet documented.
#define SCU_DMA_MODE_XFER_INITIALIZER(_len, _dst, _src) {                      \
        .len = (_len),                                                         \
        .dst = (uint32_t)(_dst),                                               \
        .src = (uint32_t)(_src)                                                \
}

/// Not yet documented.
#define SCU_DMA_MODE_XFER_END_INITIALIZER(_len, _dst, _src) {                  \
        .len = _len,                                                           \
        .dst = (uint32_t)(_dst),                                               \
        .src = DMA_INDIRECT_TBL_END | (uint32_t)(_src)                         \
}

/// @}

/// @addtogroup SCU_DMA_STRUCTURES
/// @{

/// @compound
/// @brief Used when buffering a SCU-DMA transfer with @ref scu_dma_config_buffer
/// @see scu_dma_config_buffer
struct scu_dma_reg_buffer {
        /// Not yet documented.
        uint32_t buffer[5];
} __packed __aligned(4);

/// @compound
/// @brief Defines a SCU-DMA transfer entry.
struct scu_dma_xfer {
        /// Transfer length.
        /// @note Keep in mind of the following restrictions:
        /// - Level 0 is able to transfer up to 1MiB.
        /// - Level 1 is able to transfer up to 4KiB.
        /// - Level 2 is able to transfer up to 4KiB.
        uint32_t len;
        /// Absolute destination address.
        ///
        /// Address must be uncached.
        uint32_t dst;
        /// Absolute source address.
        ///
        /// Address must be uncached.
        uint32_t src;
} __packed;

/// @compound
/// @brief Describes an instance of a SCU-DMA configuration.
///
/// Here is where you can describe how a direct transfer would work.
///
/// Here is where you can describe how an indirect transfer would work.
///
/// Here is where you would explain the issues with indirect transfer mode.
///
/// @see scu_dma_config_buffer,
///      scu_dma_config_set
struct scu_dma_level_cfg {
        /// Transfer mode.
        /// @see SCU_DMA_MODE_DIRECT,
        ///      SCU_DMA_MODE_INDIRECT
        uint8_t mode;

        /// Set either a single (direct) transfer or indirect transfer table.
        union {
                void *indirect;
                struct scu_dma_xfer direct;
        } xfer;

        /// Transfer stride.
        ///
        /// @todo Explain what SCU_DMA_STRIDE_0_BYTES is used for.
        /// @see SCU_DMA_STRIDE_0_BYTES,
        ///      SCU_DMA_STRIDE_2_BYTES,
        ///      SCU_DMA_STRIDE_4_BYTES,
        ///      SCU_DMA_STRIDE_8_BYTES,
        ///      SCU_DMA_STRIDE_16_BYTES,
        ///      SCU_DMA_STRIDE_32_BYTES,
        ///      SCU_DMA_STRIDE_64_BYTES,
        ///      SCU_DMA_STRIDE_128_BYTES
        uint8_t stride;

        /// Source and destination update mode.
        /// @see SCU_DMA_UPDATE_NONE,
        ///      SCU_DMA_UPDATE_RUP,
        ///      SCU_DMA_UPDATE_WUP
        uint32_t update;
};
/// @}

/// @addtogroup SCU_DMA_INLINE_FUNCTIONS
/// @{

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
static inline uint8_t __always_inline
scu_dma_bus_access_busy(void)
{
        return (MEMORY_READ(32, SCU(DSTA)) >> 20) & 0x03;
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_bus_access_wait(const uint8_t bus_mask)
{
        while (((scu_dma_bus_access_busy()) & bus_mask) != 0x00);
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
scu_dma_level_busy(const uint8_t level)
{
        switch (level & 0x03) {
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
scu_dma_level0_wait(void)
{
        /* Cannot modify registers while in operation */
        while ((scu_dma_level0_busy()) != 0x00000000);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level1_wait(void)
{
        /* Cannot modify registers while in operation */
        while ((scu_dma_level1_busy()) != 0x00000000);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level2_wait(void)
{
        /* Cannot modify registers while in operation */
        while ((scu_dma_level2_busy()) != 0x00000000);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level_wait(const uint8_t level)
{
        switch (level & 0x03) {
        case 0:
                scu_dma_level0_wait();
                break;
        case 1:
                scu_dma_level1_wait();
                break;
        case 2:
                scu_dma_level2_wait();
                break;
        }
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level0_fast_start(void)
{
        MEMORY_WRITE(32, SCU(D0EN), 0x00000101);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level1_fast_start(void)
{
        MEMORY_WRITE(32, SCU(D1EN), 0x00000101);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level2_fast_start(void)
{
        MEMORY_WRITE(32, SCU(D2EN), 0x00000101);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level0_start(void)
{
        scu_dma_level0_wait();
        scu_dma_level0_fast_start();
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level1_start(void)
{
        scu_dma_level1_wait();
        scu_dma_level1_fast_start();
}

/// @brief Not yet documented.
/// @warning To prevent operation errors, do not activate DMA level 2 during DMA
/// level 1 operation.
static inline void __always_inline
scu_dma_level2_start(void)
{
        scu_dma_level1_wait();
        scu_dma_level2_wait();
        scu_dma_level2_fast_start();
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level_fast_start(const uint8_t level)
{
        switch (level & 0x03) {
        case 0:
                scu_dma_level0_fast_start();
                break;
        case 1:
                scu_dma_level1_fast_start();
                break;
        case 2:
                scu_dma_level2_fast_start();
                break;
        }
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level_start(const uint8_t level)
{
        switch (level & 0x03) {
        case 0:
                scu_dma_level0_start();
                break;
        case 1:
                scu_dma_level1_start();
                break;
        case 2:
                scu_dma_level2_start();
                break;
        }
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level0_stop(void)
{
        MEMORY_WRITE(32, SCU(D0EN), 0x00000000);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level1_stop(void)
{
        MEMORY_WRITE(32, SCU(D1EN), 0x00000000);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level2_stop(void)
{
        MEMORY_WRITE(32, SCU(D2EN), 0x00000000);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level_stop(const uint8_t level)
{
        switch (level & 0x03) {
        case 0:
                scu_dma_level0_stop();
                break;
        case 1:
                scu_dma_level1_stop();
                break;
        case 2:
                scu_dma_level2_stop();
                break;
        }
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_stop(void)
{
        scu_dma_level0_stop();
        scu_dma_level1_stop();
        scu_dma_level2_stop();
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_illegal_set(void (* const ihr)(void))
{
        scu_ic_ihr_set(SCU_IC_INTERRUPT_DMA_ILLEGAL, ihr);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level0_end_set(void (* const ihr)(void))
{
        scu_ic_ihr_set(SCU_IC_INTERRUPT_LEVEL_0_DMA_END, ihr);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level1_end_set(void (* const ihr)(void))
{
        scu_ic_ihr_set(SCU_IC_INTERRUPT_LEVEL_1_DMA_END, ihr);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level2_end_set(void (* const ihr)(void))
{
        scu_ic_ihr_set(SCU_IC_INTERRUPT_LEVEL_2_DMA_END, ihr);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_dma_level_end_set(const uint8_t level, void (* const ihr)(void))
{
        switch (level & 0x03) {
        case 2:
                scu_dma_level2_end_set(ihr);
                break;
        case 1:
                scu_dma_level1_end_set(ihr);
                break;
        case 0:
        default:
                scu_dma_level0_end_set(ihr);
                break;
        }
}

/// @}

/// @addtogroup SCU_DMA_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void scu_dma_init(void);

/// @brief Not yet documented.
extern void scu_dma_config_buffer(struct scu_dma_reg_buffer *, const struct scu_dma_level_cfg *);

/// @brief Not yet documented.
extern void scu_dma_config_set(uint8_t, uint8_t, const struct scu_dma_reg_buffer *, void (*)(void));

/// @brief Determines which level is unused.
/// @returns The first unused level number in ascending order.
extern int8_t scu_dma_level_unused_get(void);

/// @}

__END_DECLS

#endif /* !_SCU_DMA_H_ */
