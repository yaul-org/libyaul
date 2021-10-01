/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCU_DMA_H_
#define _YAUL_SCU_DMA_H_

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

/// @brief The number of SCU-DMA levels.
#define SCU_DMA_LEVEL_COUNT 3

/// @brief SCU-DMA transfer mode.
typedef enum scu_dma_mode {
        /// @brief SCU-DMA direct mode.
        SCU_DMA_MODE_DIRECT   = 0x00,
        /// @brief SCU-DMA indirect mode.
        SCU_DMA_MODE_INDIRECT = 0x01
} scu_dma_mode_t;

/// @brief SCU-DMA starting factors.
typedef enum scu_dma_start_factor {
        /// @brief Start DMA transfer at VBLANK-IN.
        SCU_DMA_START_FACTOR_VBLANK_IN       = 0x00,
        /// @brief Start DMA transfer at VBLANK-OUT.
        SCU_DMA_START_FACTOR_VBLANK_OUT      = 0x01,
        /// @brief Start DMA transfer at HBLANK-IN.
        SCU_DMA_START_FACTOR_HBLANK_IN       = 0x02,
        /// @brief Start DMA transfer at SCU timer #0.
        SCU_DMA_START_FACTOR_TIMER_0         = 0x03,
        /// @brief Start DMA transfer at SCU timer #1.
        SCU_DMA_START_FACTOR_TIMER_1         = 0x04,
        /// @brief Start DMA transfer at sound request.
        SCU_DMA_START_FACTOR_SOUND_REQ       = 0x05,
        /// @brief Start DMA transfer at VDP1 sprite end.
        SCU_DMA_START_FACTOR_SPRITE_DRAW_END = 0x06,
        /// @brief Start DMA transfer immediately.
        SCU_DMA_START_FACTOR_ENABLE          = 0x07
} scu_dma_start_factor_t;

/// @brief SCU-DMA transfer stride.
typedef enum scu_dma_stride {
        /// @brief Do not add to read/write addresses.
        SCU_DMA_STRIDE_0_BYTES   = 0x00,
        /// @brief 2-byte stride.
        SCU_DMA_STRIDE_2_BYTES   = 0x01,
        /// @brief 4-byte stride.
        SCU_DMA_STRIDE_4_BYTES   = 0x02,
        /// @brief 8-byte stride.
        SCU_DMA_STRIDE_8_BYTES   = 0x03,
        /// @brief 16-byte stride.
        SCU_DMA_STRIDE_16_BYTES  = 0x04,
        /// @brief 32-byte stride.
        SCU_DMA_STRIDE_32_BYTES  = 0x05,
        /// @brief 64-byte stride.
        SCU_DMA_STRIDE_64_BYTES  = 0x06,
        /// @brief 128-byte stride.
        SCU_DMA_STRIDE_128_BYTES = 0x07
} scu_dma_stride_t;

/// @brief SCU-DMA transfer update type.
typedef enum scu_dma_update {
        /// @brief Do not update read or write address.
        SCU_DMA_UPDATE_NONE = 0x00000000UL,
        /// @brief Read address update.
        SCU_DMA_UPDATE_RUP  = 0x00010000UL,
        /// @brief Write address update.
        SCU_DMA_UPDATE_WUP  = 0x00000100UL
} scu_dma_update_t;

/// @brief The bit that signifies the end of the transfer table.
///
/// @details This bit must be set in @ref scu_dma_xfer.src.
///
/// @warning Forgetting to bitwise OR this value to the last @ref
/// scu_dma_xfer.src in the table @em will result in the machine locking up.
#define SCU_DMA_INDIRECT_TABLE_END (0x80000000UL)

/// @brief Different busses.
typedef enum scu_dma_bus {
        /// @brief None.
        SCU_DMA_BUS_NONE = 0x00,
        /// @brief A-Bus.
        SCU_DMA_BUS_A    = 0x01,
        /// @brief B-Bus.
        SCU_DMA_BUS_B    = 0x02,
        /// @brief SCU-DSP Bus.
        SCU_DMA_BUS_DSP  = 0x04
} scu_dma_bus_t;

/// @brief Initializer for @ref scu_dma_xfer_t.
///
/// @details This macro can be used in direct or indirect mode.
///
/// @param _len The length.
/// @param _dst The memory transfer destination address.
/// @param _src The memory transfer source address.
#define SCU_DMA_MODE_XFER_INITIALIZER(_len, _dst, _src)                        \
{                                                                              \
        .len = (_len),                                                         \
        .dst = (uint32_t)(_dst),                                               \
        .src = (uint32_t)(_src)                                                \
}

/// @brief End Initializer for @ref scu_dma_xfer_t.
///
/// @details Terminates the transfer table for when the level is in indirect
/// mode.
///
/// @param _len The length.
/// @param _dst The memory transfer destination address.
/// @param _src The memory transfer source address.
#define SCU_DMA_MODE_XFER_END_INITIALIZER(_len, _dst, _src)                    \
{                                                                              \
        .len = _len,                                                           \
        .dst = (uint32_t)(_dst),                                               \
        .src = SCU_DMA_INDIRECT_TABLE_END | (uint32_t)(_src)                   \
}

/// @brief SCU-DMA level representing one of the 3 levels.
typedef int32_t scu_dma_level_t;

/// @brief A SCU-DMA handle representing a copy of the level's SCU-DMA
/// registers.
///
/// @details After configuring with @ref scu_dma_config_buffer using @ref
/// scu_dma_level_cfg, the end result is a handle.
///
/// @see scu_dma_config_buffer
/// @see scu_dma_config_set
typedef struct scu_dma_handle {
        /// Value for @ref D0R, @ref D1R, @ref D2R.
        uint32_t dnr;
        /// Value for @ref D0W, @ref D1W, @ref D2W.
        uint32_t dnw;
        /// Value for @ref D0C, @ref D1C, @ref D2C.
        uint32_t dnc;
        /// Value for @ref D0C, @ref D1C, @ref D2C.
        uint32_t dnad;
        /// Value for @ref D0MD, @ref D1MD, @ref D2MD.
        uint32_t dnmd;
} __packed __aligned(4) scu_dma_handle_t;

/// @brief The 3-tuple represents a single transfer in direct or indirect
/// transfer.
///
/// @details When a SCU-DMA level is configured to operate in direct mode (@ref
/// scu_dma_level_cfg.mode), then this structure represents one transfer.
///
/// However, when the mode is @ref SCU_DMA_MODE_INDIRECT, then this structure
/// represents possibly one of many transfers contiguously laid out in memory as
/// a @em table of transfers.
typedef struct scu_dma_xfer {
        /// Transfer length.
        uint32_t len;

        /// Memory transfer destination address.
        uint32_t dst;

        /// @brief Memory transfer source address.
        ///
        /// @details When in indirect mode, the last entry @em must be bitwise
        /// OR'd with the memory transfer source address and @ref
        /// scu_dma_xfer.src with @ref SCU_DMA_INDIRECT_TABLE_END. Otherwise,
        /// the machine @em will lock up.
        uint32_t src;
/* The alignment must be 4 bytes as the entries in the table are packed
 * together */
} __packed __aligned(4) scu_dma_xfer_t;

/// The transfer type.
typedef union scu_dma_xfer_type {
        /// When indirect mode is used via @ref scu_dma_level_cfg.mode, use
        /// this to point to the transfer table.
        scu_dma_xfer_t *indirect;

        /// When indirect mode is used via @ref scu_dma_level_cfg.mode, use
        /// this to set the 3-tuple.
        scu_dma_xfer_t direct;
} scu_dma_xfer_type_t;

/// @brief Defines a SCU-DMA level configuration.
///
/// @details Details goes here.
///
/// @warning
/// + When in indirect mode,
///   - The last entry in the transfer table @em must be bitwise OR'd with the
///     memory transfer source address and @ref scu_dma_xfer.src with @ref
///     SCU_DMA_INDIRECT_TABLE_END. Otherwise, the machine @em will lock up.
///
///   - The base pointer to the table of transfers (@ref
///     scu_dma_xfer_type.indirect) must be byte aligned proportional to the
///     number of transfers in the table. Ignoring this condition @em will cause
///     the machine to lock up.
///
///     For example, if the transfer count in the table is 1, and because this
///     structure is 12 bytes, first round to the nearest power of 2, the
///     alignment of the table must be on a 16-byte boundary. If the the
///     transfer count is 3 (36 bytes rounded to 64), then the table must be on
///     a 64-byte boundary.
///
///     Effectively, the formula to determine the byte alignment is
///     @f$2^{\lceil\log_2 N\rceil}@f$-bytes where @f$N@f$ is the size of the
///     transfer table in bytes.
///
///     When allocating dynamically, use @ref memalign. Otherwise, when
///     statically allocating, use the @ref __aligned GCC attribute.
typedef struct scu_dma_level_cfg {
        /// Transfer mode.
        scu_dma_mode_t mode:8;
        /// Transfer stride.
        scu_dma_stride_t stride:8;
        /// Transfer update.
        scu_dma_update_t update;
        /// Transfer 3-tuple.
        scu_dma_xfer_type_t xfer;
} __packed __aligned(4) scu_dma_level_cfg_t;

static_assert(sizeof(scu_dma_level_cfg_t) == 20);

/// @brief Callback type.
/// @see scu_dma_illegal_set
typedef void (*scu_dma_ihr_t)(void);

/// @brief Callback type.
/// @see scu_dma_config_set
/// @see scu_dma_level_end_set
typedef void (*scu_dma_callback_t)(void *);

/// @brief Stop a specific SCU-DMA level.
///
/// @param level The SCU-DMA level.
extern void scu_dma_level_stop(scu_dma_level_t level);

/// @brief Obtain the 32-bit SCU @ref DSTA value.
/// @returns The 32-bit SCU @ref DSTA value.
static inline uint32_t __always_inline
scu_dma_status_get(void)
{
        return MEMORY_READ(32, SCU(DSTA));
}

/// @brief Obtain the status of SCU-DSP DMA.
/// @returns Whether SCU-DSP DMA is in operation, on standby, or interrupted to
/// background.
static inline uint32_t __always_inline
scu_dma_dsp_busy(void)
{
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00010003);
}

/// @brief Wait until SCU-DSP DMA is no longer in operation/standby.
static inline void __always_inline
scu_dma_dsp_wait(void)
{
        while ((scu_dma_dsp_busy()) != 0x00000000);
}

/// @brief Obtain which bus(es) are being accessed during DMA.
/// @returns The mask of bus(es) that are being accessed during DMA.
static inline scu_dma_bus_t __always_inline
scu_dma_bus_access_busy(void)
{
        return (scu_dma_bus_t)((MEMORY_READ(32, SCU(DSTA)) >> 20) & 0x07);
}

/// @brief Wait until the mask of bus(es) are no longer being accessed during
/// DMA.
///
/// @param mask The mask of bus(es).
static inline void __always_inline
scu_dma_bus_access_wait(scu_dma_bus_t mask)
{
        while (((scu_dma_bus_access_busy()) & mask) != 0x00);
}

/// @brief Obtain the status of SCU-DMA level 0.
/// @returns Whether SCU-DMA level 0 is in operation, on standby, or
/// interrupted.
static inline uint32_t __always_inline
scu_dma_level0_busy(void)
{
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00010030);
}

/// @brief Obtain the status of SCU-DMA level 1.
/// @returns Whether SCU-DMA level 1 is in operation, on standby, or
/// interrupted.
static inline uint32_t __always_inline
scu_dma_level1_busy(void)
{
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00020300);
}

/// @brief Obtain the status of SCU-DMA level 2.
/// @returns Whether SCU-DMA level 2 is in operation, on standby, or
/// interrupted.
static inline uint32_t __always_inline
scu_dma_level2_busy(void)
{
        return (MEMORY_READ(32, SCU(DSTA)) & 0x00003000);
}

/// @brief Obtain the status of SCU-DMA level.
///
/// @param level The SCU-DMA level.
///
/// @returns Whether SCU-DMA level is in operation, on standby, or interrupted.
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

/// @brief Stop all SCU-DMA levels unconditionally.
static inline void __always_inline
scu_dma_stop(void)
{
        scu_dma_level_stop(0);
        scu_dma_level_stop(1);
        scu_dma_level_stop(2);
}

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the interrupt handler for the SCU-DMA illegal interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr The interrupt handler.
static inline void __always_inline
scu_dma_illegal_set(scu_dma_ihr_t ihr)
{
        scu_ic_ihr_set(SCU_IC_INTERRUPT_DMA_ILLEGAL, ihr);
}

/// @ingroup CPU_INTC_HELPERS
/// @brief Clear the interrupt handler for the SCU-DMA illegal interrupt.
/// @see scu_dma_illegal_set
static inline void __always_inline
scu_dma_illegal_clear(void)
{
        scu_dma_illegal_set(NULL);
}

/// @brief Generates a handle from the SCU-DMA level configuration.
///
/// @param[out] handle The handle.
/// @param[in]  cfg    The configuration.
extern void scu_dma_config_buffer(scu_dma_handle_t *handle,
    const scu_dma_level_cfg_t *cfg);

/// @brief Commits handle to SCU-DMA level I/O registers.
///
/// @param     level        The SCU-DMA level.
/// @param     start_factor The starting factor.
/// @param[in] handle       The handle.
/// @param     callback     The callback when transfer is complete.
extern void scu_dma_config_set(scu_dma_level_t level,
    scu_dma_start_factor_t start_factor, const scu_dma_handle_t *handle,
    scu_dma_callback_t callback);

/// @ingroup SCU_IC_HELPERS
/// @brief Set the interrupt handler for the SCU-DMA level end interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param level    The SCU-DMA level.
/// @param callback The interrupt handler.
/// @param work     The pointer to the work passed to @p callback.
extern void scu_dma_level_end_set(scu_dma_level_t level,
    scu_dma_callback_t callback, void *work);

/// @brief Start a SCU-DMA transfer.
///
/// @details There is no check if the SCU-DMA level @p level is currently
/// operating.
///
/// There is no check if the SCU-DMA level @p level has been configured.
extern void scu_dma_level_fast_start(scu_dma_level_t level);

/// @brief Start a SCU-DMA transfer.
///
/// @details The function waits until the SCU-DMA level @p level is not in
/// operation or standby. There is also a check if @p level is 2, then it also
/// waits until SCU-DMA level 1 is not in operation or standby.
///
/// There is no check if the SCU-DMA level @p level has been configured.
extern void scu_dma_level_start(scu_dma_level_t level);

/// @brief Wait until the SCU-DMA level is not in operation or standby.
///
/// @details Waits indefinitely until the SCU-DMA level @p level is not in
/// operation or standby.
///
/// @param level The SCU-DMA level.
extern void scu_dma_level_wait(scu_dma_level_t level);

/// @brief Obtain the first unused SCU-DMA level.
///
/// @returns SCU-DMA level 0, then 1, then 2. Otherwise, -1 is returned.
extern scu_dma_level_t scu_dma_level_unused_get(void);

/// @}

__END_DECLS

#endif /* !_YAUL_SCU_DMA_H_ */
