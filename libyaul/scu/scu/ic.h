/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_IC_H_
#define _SCU_IC_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <scu/map.h>

__BEGIN_DECLS

#define SCU_IC_INTERRUPT_VBLANK_IN              0x40
#define SCU_IC_INTERRUPT_VBLANK_OUT             0x41
#define SCU_IC_INTERRUPT_HBLANK_IN              0x42
#define SCU_IC_INTERRUPT_TIMER_0                0x43
#define SCU_IC_INTERRUPT_TIMER_1                0x44
#define SCU_IC_INTERRUPT_DSP_END                0x45
#define SCU_IC_INTERRUPT_SOUND_REQUEST          0x46
#define SCU_IC_INTERRUPT_SYSTEM_MANAGER         0x47
#define SCU_IC_INTERRUPT_PAD_INTERRUPT          0x48
#define SCU_IC_INTERRUPT_LEVEL_2_DMA_END        0x49
#define SCU_IC_INTERRUPT_LEVEL_1_DMA_END        0x4A
#define SCU_IC_INTERRUPT_LEVEL_0_DMA_END        0x4B
#define SCU_IC_INTERRUPT_DMA_ILLEGAL            0x4C
#define SCU_IC_INTERRUPT_SPRITE_END             0x4D

#define SCU_IC_MASK_NONE                0x00000000
#define SCU_IC_MASK_VBLANK_IN           0x00000001
#define SCU_IC_MASK_VBLANK_OUT          0x00000002
#define SCU_IC_MASK_HBLANK_IN           0x00000004
#define SCU_IC_MASK_TIMER_0             0x00000008
#define SCU_IC_MASK_TIMER_1             0x00000010
#define SCU_IC_MASK_DSP_END             0x00000020
#define SCU_IC_MASK_SOUND_REQUEST       0x00000040
#define SCU_IC_MASK_SYSTEM_MANAGER      0x00000080
#define SCU_IC_MASK_PAD_INTERRUPT       0x00000100
#define SCU_IC_MASK_LEVEL_2_DMA_END     0x00000200
#define SCU_IC_MASK_LEVEL_1_DMA_END     0x00000400
#define SCU_IC_MASK_LEVEL_0_DMA_END     0x00000800
#define SCU_IC_MASK_DMA_ILLEGAL         0x00001000
#define SCU_IC_MASK_SPRITE_END          0x00002000
#define SCU_IC_MASK_A_BUS               0x00008000
#define SCU_IC_MASK_ALL                 0x0000BFFF

#define SCU_IC_IST_NONE                 0x00000000
#define SCU_IC_IST_VBLANK_IN            0x00000001
#define SCU_IC_IST_VBLANK_OUT           0x00000002
#define SCU_IC_IST_HBLANK_IN            0x00000004
#define SCU_IC_IST_TIMER_0              0x00000008
#define SCU_IC_IST_TIMER_1              0x00000010
#define SCU_IC_IST_DSP_END              0x00000020
#define SCU_IC_IST_SOUND_REQUEST        0x00000040
#define SCU_IC_IST_SYSTEM_MANAGER       0x00000080
#define SCU_IC_IST_PAD_INTERRUPT        0x00000100
#define SCU_IC_IST_LEVEL_2_DMA_END      0x00000200
#define SCU_IC_IST_LEVEL_1_DMA_END      0x00000400
#define SCU_IC_IST_LEVEL_0_DMA_END      0x00000800
#define SCU_IC_IST_DMA_ILLEGAL          0x00001000
#define SCU_IC_IST_SPRITE_END           0x00002000
#define SCU_IC_IST_EXTERNAL_16          0x00010000
#define SCU_IC_IST_EXTERNAL_15          0x00020000
#define SCU_IC_IST_EXTERNAL_14          0x00040000
#define SCU_IC_IST_EXTERNAL_13          0x00080000
#define SCU_IC_IST_EXTERNAL_12          0x00100000
#define SCU_IC_IST_EXTERNAL_11          0x00200000
#define SCU_IC_IST_EXTERNAL_10          0x00400000
#define SCU_IC_IST_EXTERNAL_09          0x00800000
#define SCU_IC_IST_EXTERNAL_08          0x01000000
#define SCU_IC_IST_EXTERNAL_07          0x02000000
#define SCU_IC_IST_EXTERNAL_06          0x04000000
#define SCU_IC_IST_EXTERNAL_05          0x08000000
#define SCU_IC_IST_EXTERNAL_04          0x10000000
#define SCU_IC_IST_EXTERNAL_03          0x20000000
#define SCU_IC_IST_EXTERNAL_02          0x40000000
#define SCU_IC_IST_EXTERNAL_01          0x80000000

static inline void __always_inline
scu_ic_ihr_set(uint8_t vector, void (*ihr)(void))
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000300;

        ((void (*)(uint8_t, void (*)(void)))*bios_address)(vector, ihr);
}

static inline void __always_inline
scu_ic_ihr_clear(uint8_t vector)
{
        scu_ic_ihr_set(vector, NULL);
}

static inline void __always_inline (*scu_ic_ihr_get(uint8_t vector))(void)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000304;

        return ((void (*(*)(uint8_t))(void))*bios_address)(vector);
}

static inline void __always_inline
scu_ic_mask_set(uint32_t mask)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000340;

        ((void (*)(uint32_t))*bios_address)(mask);
}

static inline void __always_inline
scu_ic_mask_chg(uint32_t and_mask, uint32_t or_mask)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000344;

        ((void (*)(uint32_t, uint32_t))*bios_address)(and_mask, or_mask);
}

static inline uint32_t __always_inline
scu_ic_mask_get(void)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000348;

        return *bios_address;
}

static inline uint32_t __always_inline
scu_ic_status_get(void)
{
        uint32_t ist;
        ist = MEMORY_READ(32, SCU(IST));

        return ist;
}

static inline void __always_inline
scu_ic_status_chg(uint32_t and_mask, uint32_t or_mask)
{
        volatile uint32_t *reg_ist;
        reg_ist = (volatile uint32_t *)SCU(IST);

        *reg_ist = (*reg_ist & and_mask) | or_mask;
}

static inline void __always_inline
scu_ic_status_set(uint32_t value)
{
        MEMORY_WRITE(32, SCU(IST), value);
}

static inline const uint32_t * __always_inline
scu_ic_priority_table_get(void)
{
        /* Priority table is 0x80 bytes */
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000A80;

        return bios_address;
}

static inline void __always_inline
scu_ic_priority_table_set(uint32_t *table)
{
        /* Priority table is 0x80 bytes */
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000280;

        ((void (*)(uint32_t *))*bios_address)(table);
}

__END_DECLS

#endif /* !_SCU_IC_H_ */
