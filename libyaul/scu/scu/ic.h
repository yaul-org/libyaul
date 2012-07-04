/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _IC_H_
#define _IC_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IC_INTERRUPT_VBLANK_IN          0x40
#define IC_INTERRUPT_VBLANK_OUT         0x41
#define IC_INTERRUPT_HBLANK_IN          0x42
#define IC_INTERRUPT_TIMER_0            0x43
#define IC_INTERRUPT_TIMER_1            0x44
#define IC_INTERRUPT_DSP_END            0x45
#define IC_INTERRUPT_SOUND_REQUEST      0x46
#define IC_INTERRUPT_SYSTEM_MANAGER     0x47
#define IC_INTERRUPT_PAD_INTERRUPT      0x48
#define IC_INTERRUPT_LEVEL_2_DMA_END    0x49
#define IC_INTERRUPT_LEVEL_1_DMA_END    0x4A
#define IC_INTERRUPT_LEVEL_0_DMA_END    0x4B
#define IC_INTERRUPT_DMA_ILLEGAL        0x4C
#define IC_INTERRUPT_SPRITE_END         0x4D

#define IC_MASK_NONE            0x0000
#define IC_MASK_VBLANK_IN       0x0001
#define IC_MASK_VBLANK_OUT      0x0002
#define IC_MASK_HBLANK_IN       0x0004
#define IC_MASK_TIMER_0         0x0008
#define IC_MASK_TIMER_1         0x0010
#define IC_MASK_DSP_END         0x0020
#define IC_MASK_SOUND_REQUEST   0x0040
#define IC_MASK_SYSTEM_MANAGER  0x0080
#define IC_MASK_PAD_INTERRUPT   0x0100
#define IC_MASK_LEVEL_2_DMA_END 0x0200
#define IC_MASK_LEVEL_1_DMA_END 0x0400
#define IC_MASK_LEVEL_0_DMA_END 0x0800
#define IC_MASK_DMA_ILLEGAL     0x1000
#define IC_MASK_SPRITE_END      0x2000
#define IC_MASK_ALL             0xBFFF

#define IC_IST_VBLANK_IN        0x00000001
#define IC_IST_VBLANK_OUT       0x00000002
#define IC_IST_HBLANK_IN        0x00000004
#define IC_IST_TIMER_0          0x00000008
#define IC_IST_TIMER_1          0x00000010
#define IC_IST_DSP_END          0x00000020
#define IC_IST_SOUND_REQUEST    0x00000040
#define IC_IST_SYSTEM_MANAGER   0x00000080
#define IC_IST_PAD_INTERRUPT    0x00000100
#define IC_IST_LEVEL_2_DMA_END  0x00000200
#define IC_IST_LEVEL_1_DMA_END  0x00000400
#define IC_IST_LEVEL_0_DMA_END  0x00000800
#define IC_IST_DMA_ILLEGAL      0x00001000
#define IC_IST_SPRITE_END       0x00002000

extern uint32_t scu_ic_interrupt_get(void);
extern uint32_t scu_ic_status_get(void);
extern void scu_ic_interrupt_set(uint8_t, void (*)(void));
extern void scu_ic_mask_chg(uint16_t, uint16_t);
extern void scu_ic_mask_set(uint16_t, uint16_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_IC_H_ */
