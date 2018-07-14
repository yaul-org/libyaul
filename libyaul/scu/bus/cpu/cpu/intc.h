/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_INTC_H_
#define _CPU_INTC_H_

#include <stdint.h>

#include <cpu/registers.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define INTC_INTERRUPT_ILLEGAL_INSTRUCTION      0x04
#define INTC_INTERRUPT_ILLEGAL_SLOT             0x06
#define INTC_INTERRUPT_CPU_ADDRESS_ERROR        0x09
#define INTC_INTERRUPT_DMA_ADDRESS_ERROR        0x0A
#define INTC_INTERRUPT_UBC                      0x0C
#define INTC_INTERRUPT_BREAK                    0x20
#define INTC_INTERRUPT_DIVU_OVFI                0x50
#define INTC_INTERRUPT_DMAC0                    0x51
#define INTC_INTERRUPT_DMAC1                    0x52
#define INTC_INTERRUPT_WDT_ITI                  0x53
#define INTC_INTERRUPT_BSC                      0x54
#define INTC_INTERRUPT_SCI_ERI                  0x55
#define INTC_INTERRUPT_SCI_RXI                  0x56
#define INTC_INTERRUPT_SCI_TXI                  0x57
#define INTC_INTERRUPT_SCI_TEI                  0x58
#define INTC_INTERRUPT_FRT_ICI                  0x59
#define INTC_INTERRUPT_FRT_OCI                  0x5A
#define INTC_INTERRUPT_FRT_OVI                  0x5B
#define INTC_INTERRUPT_UNKNOWN_A                0x5C
#define INTC_INTERRUPT_UNKNOWN_B                0x5D
#define INTC_INTERRUPT_UNKNOWN_C                0x5E
#define INTC_INTERRUPT_UNKNOWN_D                0x5F

#define INTC_PRIORITY_VBLANK_IN         15
#define INTC_PRIORITY_VBLANK_OUT        14
#define INTC_PRIORITY_HBLANK_IN         13
#define INTC_PRIORITY_TIMER_0           12
#define INTC_PRIORITY_TIMER_1           11
#define INTC_PRIORITY_DSP_END           10
#define INTC_PRIORITY_SOUND_REQUEST     9
#define INTC_PRIORITY_SYSTEM_MANAGER    8
#define INTC_PRIORITY_PAD_INTERRUPT     8
#define INTC_PRIORITY_LEVEL_2_DMA_END   6
#define INTC_PRIORITY_LEVEL_1_DMA_END   6
#define INTC_PRIORITY_LEVEL_0_DMA_END   5
#define INTC_PRIORITY_DMA_ILLEGAL       3
#define INTC_PRIORITY_SPRITE_END        2

static inline void __attribute__ ((always_inline))
cpu_intc_ihr_set(uint8_t vector, void (*ihr)(void))
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000310;

        ((void (*)(uint8_t, void (*)(void)))*bios_address)(vector, ihr);
}

static inline void __attribute__ ((always_inline)) (*cpu_intc_ihr_get(uint8_t vector))(void)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000314;

        return ((void (*(*)(uint8_t))(void))*bios_address)(vector);
}

static inline uint32_t __attribute__ ((always_inline))
cpu_intc_mask_get(void)
{
        register uint32_t reg_sr;
        reg_sr = cpu_reg_sr_get();

        register uint32_t mask;
        mask = (reg_sr >> 4) & 0x0F;

        return mask;
}

static inline void __attribute__ ((always_inline))
cpu_intc_mask_set(uint8_t mask)
{
        register uint32_t reg_sr;
        reg_sr = cpu_reg_sr_get();

        reg_sr &= 0xFFFFFF0F;
        reg_sr |= (mask & 0x0F) << 4;

        cpu_reg_sr_set(reg_sr);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_INTC_H */
