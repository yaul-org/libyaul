/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _IC_H_
#define _IC_H_

#include <inttypes.h>

enum ic_vct_type {
        IC_VCT_VBLANK_IN = 0x40,        /* Interrupt Source: VDP2 */
        IC_VCT_VBLANK_OUT = 0x41,       /* Interrupt Source: VDP2 */
        IC_VCT_HBLANK_IN = 0x42,        /* Interrupt Source: VDP2 */
        IC_VCT_TIMER_0 = 0x43,          /* Interrupt Source: SCU */
        IC_VCT_TIMER_1 = 0x44,          /* Interrupt Source: SCU */
        IC_VCT_DSP_END = 0x45,          /* Interrupt Source: SCU */
        IC_VCT_SOUND_REQUEST = 0x46,    /* Interrupt Source: SCSP */
        IC_VCT_SYSTEM_MANAGER = 0x47,   /* Interrupt Source: SM */
        IC_VCT_PAD_INTERRUPT = 0x48,    /* Interrupt Source: PAD */
        IC_VCT_LEVEL_2_DMA_END = 0x49,  /* Interrupt Source: A-BUS */
        IC_VCT_LEVEL_1_DMA_END = 0x4a,  /* Interrupt Source: A-BUS */
        IC_VCT_LEVEL_0_DMA_END = 0x4b,  /* Interrupt Source: A-BUS */
        IC_VCT_DMA_ILLEGAL = 0x4c,      /* Interrupt Source: SCU */
        IC_VCT_SPRITE_END = 0x4d        /* Interrupt Source: VDP1 */
};

enum ic_msk_type {
        IC_MSK_NULL = 0x0000,
        IC_MSK_VBLANK_IN = 0x0001,
        IC_MSK_VBLANK_OUT = 0x0002,
        IC_MSK_HBLANK_IN = 0x0004,
        IC_MSK_TIMER_0 = 0x0008,
        IC_MSK_TIMER_1 = 0x0010,
        IC_MSK_DSP_END = 0x0020,
        IC_MSK_SOUND_REQUEST = 0x0040,
        IC_MSK_SYSTEM_MANAGER = 0x0080,
        IC_MSK_PAD_INTERRUPT = 0x0100,
        IC_MSK_LEVEL_2_DMA_END = 0x0200,
        IC_MSK_LEVEL_1_DMA_END = 0x0400,
        IC_MSK_LEVEL_0_DMA_END = 0x0800,
        IC_MSK_DMA_ILLEGAL = 0x1000,
        IC_MSK_SPRITE_END = 0x2000,
        IC_MSK_ALL = 0xbfff
};

extern void             scu_ic_lvl_set(unsigned char);
extern void             scu_ic_msk_chg(enum ic_msk_type, enum ic_msk_type);
extern void             scu_ic_msk_set(enum ic_msk_type, enum ic_msk_type);
extern unsigned long    scu_ic_vct_get(void);
extern void             scu_ic_vct_set(enum ic_vct_type, void(*)(void));

#endif /* !_IC_H_ */
