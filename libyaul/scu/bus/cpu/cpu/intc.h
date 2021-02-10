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

/// @defgroup CPU_INTC_DEFINES CPU INTC defines
/// @defgroup CPU_INTC_INTERRUPT_DEFINES CPU INTC interrupt defines
/// @defgroup CPU_INTC_PRIORITY_DEFINES CPU INTC priority defines
/// @defgroup CPU_INTC_INLINE_FUNCTIONS CPU INTC inline functions

/// @addtogroup CPU_INTC_DEFINES
/// @{

/// Not yet documented.
#define CPU_INTC_INTERRUPT_MASTER_BASE  0x0000
/// Not yet documented.
#define CPU_INTC_INTERRUPT_SLAVE_BASE   0x0100

/// @}

/// @addtogroup CPU_INTC_INTERRUPT_DEFINES
/// @{

/// Not yet documented.
#define CPU_INTC_INTERRUPT_POWER_ON_RESET_PC    0x00
/// Not yet documented.
#define CPU_INTC_INTERRUPT_POWER_ON_RESET_SP    0x01

/// Not yet documented.
#define CPU_INTC_INTERRUPT_MANUAL_RESET_PC      0x02
/// Not yet documented.
#define CPU_INTC_INTERRUPT_MANUAL_RESET_SP      0x03

/// Not yet documented.
#define CPU_INTC_INTERRUPT_ILLEGAL_INSTRUCTION  0x04
/// Not yet documented.
#define CPU_INTC_INTERRUPT_ILLEGAL_SLOT         0x06
/// Not yet documented.
#define CPU_INTC_INTERRUPT_CPU_ADDRESS_ERROR    0x09
/// Not yet documented.
#define CPU_INTC_INTERRUPT_DMA_ADDRESS_ERROR    0x0A

/// Not yet documented.
#define CPU_INTC_INTERRUPT_NMI                  0x0B

/// Not yet documented.
#define CPU_INTC_INTERRUPT_UBC                  0x0C

/// Not yet documented.
#define CPU_INTC_INTERRUPT_BREAK                0x20

/// Not yet documented.
#define CPU_INTC_INTERRUPT_SCI_ERI      0x60
/// Not yet documented.
#define CPU_INTC_INTERRUPT_SCI_RXI      0x61
/// Not yet documented.
#define CPU_INTC_INTERRUPT_SCI_TXI      0x62
/// Not yet documented.
#define CPU_INTC_INTERRUPT_SCI_TEI      0x63
/// Not yet documented.
#define CPU_INTC_INTERRUPT_FRT_ICI      0x64
/// Not yet documented.
#define CPU_INTC_INTERRUPT_FRT_OCI      0x65
/// Not yet documented.
#define CPU_INTC_INTERRUPT_FRT_OVI      0x66
/// Not yet documented.
#define CPU_INTC_INTERRUPT_FREE_67      0x67
/// Not yet documented.
#define CPU_INTC_INTERRUPT_WDT_ITI      0x68
/// Not yet documented.
#define CPU_INTC_INTERRUPT_BSC          0x69
/// Not yet documented.
#define CPU_INTC_INTERRUPT_FREE_6A      0x6A
/// Not yet documented.
#define CPU_INTC_INTERRUPT_FREE_6B      0x6B
/// Not yet documented.
#define CPU_INTC_INTERRUPT_DMAC0        0x6C
/// Not yet documented.
#define CPU_INTC_INTERRUPT_DMAC1        0x6D
/// Not yet documented.
#define CPU_INTC_INTERRUPT_DIVU_OVFI    0x6E
/// Not yet documented.
#define CPU_INTC_INTERRUPT_FREE_6F      0x6F
/// Not yet documented.
#define CPU_INTC_INTERRUPT_SLAVE_ENTRY  0x94

/// @}

/// @addtogroup CPU_INTC_PRIORITY_DEFINES
/// @{

/// Not yet documented.
#define CPU_INTC_PRIORITY_VBLANK_IN             15
/// Not yet documented.
#define CPU_INTC_PRIORITY_VBLANK_OUT            14
/// Not yet documented.
#define CPU_INTC_PRIORITY_HBLANK_IN             13
/// Not yet documented.
#define CPU_INTC_PRIORITY_TIMER_0               12
/// Not yet documented.
#define CPU_INTC_PRIORITY_TIMER_1               11
/// Not yet documented.
#define CPU_INTC_PRIORITY_DSP_END               10
/// Not yet documented.
#define CPU_INTC_PRIORITY_SOUND_REQUEST         9
/// Not yet documented.
#define CPU_INTC_PRIORITY_SYSTEM_MANAGER        8
/// Not yet documented.
#define CPU_INTC_PRIORITY_PAD_INTERRUPT         8
/// Not yet documented.
#define CPU_INTC_PRIORITY_LEVEL_2_DMA           6
/// Not yet documented.
#define CPU_INTC_PRIORITY_LEVEL_1_DMA           6
/// Not yet documented.
#define CPU_INTC_PRIORITY_LEVEL_0_DMA           5
/// Not yet documented.
#define CPU_INTC_PRIORITY_DMA_ILLEGAL           3
/// Not yet documented.
#define CPU_INTC_PRIORITY_SPRITE_END            2

/// @}

typedef void (*cpu_intc_ihr)(void);

/// @addtogroup CPU_INTC_INLINE_FUNCTIONS
/// @{

/// @brief Not yet documented.
static inline void __always_inline
cpu_intc_ihr_set(uint32_t vector, cpu_intc_ihr ihr)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000310;

        ((void (*)(uint32_t, void (*)(void)))*bios_address)(vector, ihr);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_intc_ihr_clear(uint32_t vector)
{
        cpu_intc_ihr_set(vector, NULL);
}

/// @brief Not yet documented.
static inline cpu_intc_ihr __always_inline
cpu_intc_ihr_get(uint32_t vector)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000314;

        return ((void (*(*)(uint32_t))(void))*bios_address)(vector);
}

/// @brief Not yet documented.
static inline uint8_t __always_inline
cpu_intc_mask_get(void)
{
        register uint32_t reg_sr;
        reg_sr = cpu_reg_sr_get();

        register uint32_t mask;
        mask = (reg_sr >> 4) & 0x0F;

        return mask;
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_intc_mask_set(uint8_t mask)
{
        register uint32_t reg_sr;
        reg_sr = cpu_reg_sr_get();

        reg_sr &= 0xFFFFFF0F;
        reg_sr |= (mask & 0x0F) << 4;

        cpu_reg_sr_set(reg_sr);
}

/// @brief Not yet documented.
static inline uint16_t __always_inline
cpu_intc_priority_a_get(void)
{
        return MEMORY_READ(16, CPU(IPRA));
}

/// @brief Not yet documented.
static inline uint16_t __always_inline
cpu_intc_priority_b_get(void)
{
        return MEMORY_READ(16, CPU(IPRB));
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_intc_priority_a_set(uint16_t ipra)
{
        MEMORY_WRITE(16, CPU(IPRA), ipra);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_intc_priority_b_set(uint16_t iprb)
{
        MEMORY_WRITE(16, CPU(IPRA), iprb);
}

/// @}

__END_DECLS

#endif /* !_CPU_INTC_H */
