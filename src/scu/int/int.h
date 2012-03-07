/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _INT_H_
#define _INT_H_

enum int_vct_type {
        INT_VCT_VBLANK_IN = 0x40,        /* Interrupt Source: VDP2 */
        INT_VCT_VBLANK_OUT = 0x41,       /* Interrupt Source: VDP2 */
	INT_VCT_HBLANK_IN = 0x42,        /* Interrupt Source: VDP2 */
	INT_VCT_TIMER_0 = 0x43,          /* Interrupt Source: SCU */
	INT_VCT_TIMER_1 = 0x44,          /* Interrupt Source: SCU */
	INT_VCT_DSP_END = 0x45,          /* Interrupt Source: SCU */
	INT_VCT_SOUND_REQUEST = 0x46,    /* Interrupt Source: SCSP */
	INT_VCT_SYSTEM_MANAGER = 0x47,   /* Interrupt Source: SM */
	INT_VCT_PAD_INTERRUPT = 0x48,    /* Interrupt Source: PAD */
	INT_VCT_LEVEL_2_DMA_END = 0x49,  /* Interrupt Source: A-BUS */
	INT_VCT_LEVEL_1_DMA_END = 0x4a,  /* Interrupt Source: A-BUS */
	INT_VCT_LEVEL_0_DMA_END = 0x4b,  /* Interrupt Source: A-BUS */
	INT_VCT_DMA_ILLEGAL = 0x4c,      /* Interrupt Source: SCU */
        INT_VCT_SPRITE_END = 0x4d        /* Interrupt Source: VDP1 */
};

enum int_msk_type {
        INT_MSK_NULL = 0x0000,
	INT_MSK_VBLANK_IN = 0x0001,
	INT_MSK_VBLANK_OUT = 0x0002,
	INT_MSK_HBLANK_IN = 0x0004,
	INT_MSK_TIMER_0 = 0x0008,
	INT_MSK_TIMER_1 = 0x0010,
	INT_MSK_DSP_END = 0x0020,
	INT_MSK_SOUND_REQUEST = 0x0040,
	INT_MSK_SYSTEM_MANAGER = 0x0080,
	INT_MSK_PAD_INTERRUPT = 0x0100,
	INT_MSK_LEVEL_2_DMA_END = 0x0200,
        INT_MSK_LEVEL_1_DMA_END = 0x0400,
	INT_MSK_LEVEL_0_DMA_END = 0x0800,
	INT_MSK_DMA_ILLEGAL = 0x1000,
	INT_MSK_SPRITE_END = 0x2000,
	INT_MSK_ALL = 0xbfff
};

extern void             scu_int_lvl_set(unsigned char);
extern void             scu_int_msk_chg(enum int_msk_type, enum int_msk_type);
extern void             scu_int_msk_set(enum int_msk_type, enum int_msk_type);
extern unsigned long    scu_int_vct_get(void);
extern void             scu_int_vct_set(enum int_vct_type, void *);

#endif /* !_INT_H_ */
