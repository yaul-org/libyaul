/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*-
 * SCU DMA should be activated by the Master CPU
 *
 * Keep in mind that:
 *   1. DMA transfers does not begin until it is explictily started
 *   2. DMA transfer level 2 will block the CPU during operation of level 1
 *
 * SCU DMA is for transfers between different buses:
 * Work RAM-H <-> A-bus
 * Work RAM-H <-> B-bus
 * A-bus <-> B-bus
 *
 * 1. A-bus write prohibited
 * 2. VDP2 area read prohibited
 * 4. WORKRAM-L usage from SCU disabled
 *
 * 2. Restriction on write address addition value in DMA based on access
 *    address
 *
 *    WORKRAM-H                        -> 0x02 can be set
 *    External areas 1 to 3            -> 0x02 can be set
 *    External area 4 (A-bus I/O area) -> 0x00 and 0x02 can be set
 *    VDP1, VDP2, SCSP                 -> all values can be set
 */

#define DMA_MODE_DIRECT         0x00
#define DMA_MODE_INDIRECT       0x01

#define DMA_MODE_START_FACTOR_VBLANK_IN         0x00
#define DMA_MODE_START_FACTOR_VBLANK_OUT        0x01
#define DMA_MODE_START_FACTOR_HBLANK_IN         0x02
#define DMA_MODE_START_FACTOR_TIMER_0           0x03
#define DMA_MODE_START_FACTOR_TIMER_1           0x04
#define DMA_MODE_START_FACTOR_SOUND_REQ         0x05
#define DMA_MODE_START_FACTOR_SPRITE_DRAW_END   0x06
#define DMA_MODE_START_FACTOR_ENABLE            0x07

#define DMA_DST_STRIDE_0_BYTES     0x00
#define DMA_DST_STRIDE_2_BYTES     0x01
#define DMA_DST_STRIDE_4_BYTES     0x02
#define DMA_DST_STRIDE_8_BYTES     0x03
#define DMA_DST_STRIDE_16_BYTES    0x04
#define DMA_DST_STRIDE_32_BYTES    0x05
#define DMA_DST_STRIDE_64_BYTES    0x06
#define DMA_DST_STRIDE_128_BYTES   0x07

#define DMA_UPDATE_NONE 0x000
#define DMA_UPDATE_RUP  0x100
#define DMA_UPDATE_WUP  0x001

#define DMA_MODE_XFER_INITIALIZER(_len, _dst, _src) {                          \
        .len = (_len),                                                         \
        .dst = (uint32_t)(_dst),                                               \
        .src = (uint32_t)(_src)                                                \
}

struct dma_xfer {
        uint32_t len;
        uint32_t dst;
        uint32_t src;
} __packed;

struct dma_level_cfg {
        uint8_t dlc_level;
        uint8_t dlc_mode;

        struct dma_xfer *dlc_xfer;
        uint16_t dlc_xfer_count;

        uint8_t dlc_stride;
        uint8_t dlc_update;
        uint8_t dlc_starting_factor;

        void (*dlc_ihr)(void);
};

static inline void __attribute__ ((always_inline))
scu_dma_level0_wait(void)
{
        /* Cannot modify registers while in operation */
        while (((MEMORY_READ(32, SCU(DSTA)) & 0x00000010) != 0x00000000) ||
               ((MEMORY_READ(32, SCU(DSTA)) & 0x00000020) != 0x00000000));
}

static inline void __attribute__ ((always_inline))
scu_dma_level1_wait(void)
{
        /* Cannot modify registers while in operation */
        while (((MEMORY_READ(32, SCU(DSTA)) & 0x00000100) != 0x00000000) ||
               ((MEMORY_READ(32, SCU(DSTA)) & 0x00080000) != 0x00000000));
}

static inline void __attribute__ ((always_inline))
scu_dma_level2_wait(void)
{
        /* To prevent operation errors, do not activate DMA
         * level 2 during DMA level 1 operation. */

        /* Cannot modify registers while in operation */
        while (((MEMORY_READ(32, SCU(DSTA)) & 0x00001000) != 0x00000000) ||
               ((MEMORY_READ(32, SCU(DSTA)) & 0x00002000) != 0x00000000));
}

static inline void __attribute__ ((always_inline))
scu_dma_level_wait(uint8_t level)
{
        switch (level) {
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

static inline void __attribute__ ((always_inline))
scu_dma_level0_enable(void)
{
        scu_dma_level0_wait();

        MEMORY_WRITE(32, SCU(D0EN), 0x00000100);
}

static inline void __attribute__ ((always_inline))
scu_dma_level1_enable(void)
{
        scu_dma_level1_wait();

        MEMORY_WRITE(32, SCU(D1EN), 0x00000100);
}

static inline void __attribute__ ((always_inline))
scu_dma_level2_enable(void)
{
        scu_dma_level2_wait();

        MEMORY_WRITE(32, SCU(D2EN), 0x00000100);
}

static inline void __attribute__ ((always_inline))
scu_dma_level_enable(uint8_t level)
{
        /* Clear starting bit, but enable the DMA level. Other required
         * data must be set in advance since DMA begins (depending on
         * the starting factor) after this bit is set. */

        switch (level) {
        case 0:
                scu_dma_level0_enable();
                break;
        case 1:
                scu_dma_level1_enable();
                break;
        case 2:
                scu_dma_level2_enable();
                break;
        }
}

static inline void __attribute__ ((always_inline))
scu_dma_level0_start(void)
{
        scu_dma_level0_wait();

        MEMORY_WRITE_OR(32, SCU(D0EN), 0x00000101);
}

static inline void __attribute__ ((always_inline))
scu_dma_level1_start(void)
{
        scu_dma_level1_wait();

        MEMORY_WRITE_OR(32, SCU(D1EN), 0x00000101);
}

static inline void __attribute__ ((always_inline))
scu_dma_level2_start(void)
{
        scu_dma_level2_wait();

        MEMORY_WRITE_OR(32, SCU(D2EN), 0x00000101);
}

static inline void __attribute__ ((always_inline))
scu_dma_level_start(uint8_t level)
{
        switch (level) {
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

static inline void __attribute__ ((always_inline))
scu_dma_level0_stop(void)
{
        MEMORY_WRITE(32, SCU(D0EN), 0x00000000);
}

static inline void __attribute__ ((always_inline))
scu_dma_level1_stop(void)
{
        MEMORY_WRITE(32, SCU(D1EN), 0x00000000);
}

static inline void __attribute__ ((always_inline))
scu_dma_level2_stop(void)
{
        MEMORY_WRITE(32, SCU(D2EN), 0x00000000);
}

static inline void __attribute__ ((always_inline))
scu_dma_level_stop(uint8_t level)
{
        switch (level) {
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

extern void scu_dma_init(void);
extern void scu_dma_level_config_set(const struct dma_level_cfg *);
extern int32_t scu_dma_level_config_validate(const struct dma_level_cfg *);
extern void cpu_dma_illegal_set(void (*)(void));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_SCU_DMA_H_ */
