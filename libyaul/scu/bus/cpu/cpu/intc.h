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

#define INTC_INTERRUPT_SCI_ERI                  0x60
#define INTC_INTERRUPT_SCI_RXI                  0x61
#define INTC_INTERRUPT_SCI_TXI                  0x62
#define INTC_INTERRUPT_SCI_TEI                  0x63
#define INTC_INTERRUPT_FRT_ICI                  0x64
#define INTC_INTERRUPT_FRT_OCI                  0x65
#define INTC_INTERRUPT_FRT_OVI                  0x66
#define INTC_INTERRUPT_FREE_67                  0x67
#define INTC_INTERRUPT_WDT_ITI                  0x68
#define INTC_INTERRUPT_BSC                      0x69
#define INTC_INTERRUPT_FREE_6A                  0x6A
#define INTC_INTERRUPT_FREE_6B                  0x6B
#define INTC_INTERRUPT_DMAC0                    0x6C
#define INTC_INTERRUPT_DMAC1                    0x6D
#define INTC_INTERRUPT_DIVU_OVFI                0x6E
#define INTC_INTERRUPT_FREE_6F                  0x6F
#define INTC_INTERRUPT_SLAVE_ENTRY              0x94

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
