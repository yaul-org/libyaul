/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_INTC_H_
#define _CPU_INTC_H_

#include <stdint.h>

#include <cpu/registers.h>
#include <cpu/dual.h>

__BEGIN_DECLS

#define CPU_INTC_INTERRUPT_POWER_ON_RESET_PC    0x00
#define CPU_INTC_INTERRUPT_POWER_ON_RESET_SP    0x01

#define CPU_INTC_INTERRUPT_MANUAL_RESET_PC      0x02
#define CPU_INTC_INTERRUPT_MANUAL_RESET_SP      0x03

#define CPU_INTC_INTERRUPT_ILLEGAL_INSTRUCTION  0x04
#define CPU_INTC_INTERRUPT_ILLEGAL_SLOT         0x06
#define CPU_INTC_INTERRUPT_CPU_ADDRESS_ERROR    0x09
#define CPU_INTC_INTERRUPT_DMA_ADDRESS_ERROR    0x0A

#define CPU_INTC_INTERRUPT_NMI                  0x0B

#define CPU_INTC_INTERRUPT_UBC                  0x0C
#define CPU_INTC_INTERRUPT_BREAK                0x20

#define CPU_INTC_INTERRUPT_SCI_ERI      0x60
#define CPU_INTC_INTERRUPT_SCI_RXI      0x61
#define CPU_INTC_INTERRUPT_SCI_TXI      0x62
#define CPU_INTC_INTERRUPT_SCI_TEI      0x63
#define CPU_INTC_INTERRUPT_FRT_ICI      0x64
#define CPU_INTC_INTERRUPT_FRT_OCI      0x65
#define CPU_INTC_INTERRUPT_FRT_OVI      0x66
#define CPU_INTC_INTERRUPT_FREE_67      0x67
#define CPU_INTC_INTERRUPT_WDT_ITI      0x68
#define CPU_INTC_INTERRUPT_BSC          0x69
#define CPU_INTC_INTERRUPT_FREE_6A      0x6A
#define CPU_INTC_INTERRUPT_FREE_6B      0x6B
#define CPU_INTC_INTERRUPT_DMAC0        0x6C
#define CPU_INTC_INTERRUPT_DMAC1        0x6D
#define CPU_INTC_INTERRUPT_DIVU_OVFI    0x6E
#define CPU_INTC_INTERRUPT_FREE_6F      0x6F
#define CPU_INTC_INTERRUPT_SLAVE_ENTRY  0x94

#define CPU_INTC_INTERRUPT_MASTER_BASE  0x0000
#define CPU_INTC_INTERRUPT_SLAVE_BASE   0x0100

#define CPU_INTC_PRIORITY_VBLANK_IN             15
#define CPU_INTC_PRIORITY_VBLANK_OUT            14
#define CPU_INTC_PRIORITY_HBLANK_IN             13
#define CPU_INTC_PRIORITY_TIMER_0               12
#define CPU_INTC_PRIORITY_TIMER_1               11
#define CPU_INTC_PRIORITY_DSP_END               10
#define CPU_INTC_PRIORITY_SOUND_REQUEST         9
#define CPU_INTC_PRIORITY_SYSTEM_MANAGER        8
#define CPU_INTC_PRIORITY_PAD_INTERRUPT         8
#define CPU_INTC_PRIORITY_LEVEL_2_DMA           6
#define CPU_INTC_PRIORITY_LEVEL_1_DMA           6
#define CPU_INTC_PRIORITY_LEVEL_0_DMA           5
#define CPU_INTC_PRIORITY_DMA_ILLEGAL           3
#define CPU_INTC_PRIORITY_SPRITE_END            2

static inline void __always_inline
cpu_intc_ihr_set(uint32_t vector, void (*ihr)(void))
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000310;

        ((void (*)(uint32_t, void (*)(void)))*bios_address)(vector, ihr);
}

static inline void __always_inline
cpu_intc_ihr_clear(uint32_t vector)
{
        cpu_intc_ihr_set(vector, NULL);
}

static inline void __always_inline (*cpu_intc_ihr_get(uint32_t vector))(void)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000314;

        return ((void (*(*)(uint32_t))(void))*bios_address)(vector);
}

static inline uint8_t __always_inline
cpu_intc_mask_get(void)
{
        register uint32_t reg_sr;
        reg_sr = cpu_reg_sr_get();

        register uint32_t mask;
        mask = (reg_sr >> 4) & 0x0F;

        return mask;
}

static inline void __always_inline
cpu_intc_mask_set(uint8_t mask)
{
        register uint32_t reg_sr;
        reg_sr = cpu_reg_sr_get();

        reg_sr &= 0xFFFFFF0F;
        reg_sr |= (mask & 0x0F) << 4;

        cpu_reg_sr_set(reg_sr);
}

__END_DECLS

#endif /* !_CPU_INTC_H */
