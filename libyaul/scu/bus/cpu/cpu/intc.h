/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_INTC_H_
#define _YAUL_CPU_INTC_H_

#include <stdint.h>

#include <cpu/registers.h>
#include <cpu/dual.h>

__BEGIN_DECLS

/// @defgroup CPU_INTC CPU Interrupt Controller (INTC)

/// @defgroup CPU_INTC_HELPERS CPU INTC Setters
/// These functions set the interrupt handler for their respective CPU INTC
/// interrupt.

/// @addtogroup CPU_INTC
/// @{

/// Interrupt vector offset for master CPU.
#define CPU_INTC_INTERRUPT_MASTER_BASE  0x0000
/// Interrupt vector offset for slave CPU.
#define CPU_INTC_INTERRUPT_SLAVE_BASE   0x0100

/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_VBLANK_IN             15
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_VBLANK_OUT            14
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_HBLANK_IN             13
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_TIMER_0               12
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_TIMER_1               11
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_DSP_END               10
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_SOUND_REQUEST         9
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_SYSTEM_MANAGER        8
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_PAD_INTERRUPT         8
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_LEVEL_2_DMA           6
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_LEVEL_1_DMA           6
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_LEVEL_0_DMA           5
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_DMA_ILLEGAL           3
/// Interrupts belonging to this priority.
#define CPU_INTC_PRIORITY_SPRITE_END            2

/// @brief Interrupt vectors.
typedef enum cpu_intc_interrupt {
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_POWER_ON_RESET_PC   = 0x00,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_POWER_ON_RESET_SP   = 0x01,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_MANUAL_RESET_PC     = 0x02,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_MANUAL_RESET_SP     = 0x03,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_ILLEGAL_INSTRUCTION = 0x04,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_ILLEGAL_SLOT        = 0x06,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_CPU_ADDRESS_ERROR   = 0x09,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_DMA_ADDRESS_ERROR   = 0x0A,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_NMI                 = 0x0B,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_UBC                 = 0x0C,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_BREAK               = 0x20,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_SCI_ERI             = 0x60,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_SCI_RXI             = 0x61,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_SCI_TXI             = 0x62,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_SCI_TEI             = 0x63,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_FRT_ICI             = 0x64,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_FRT_OCI             = 0x65,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_FRT_OVI             = 0x66,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_FREE_67             = 0x67,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_WDT_ITI             = 0x68,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_BSC                 = 0x69,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_FREE_6A             = 0x6A,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_FREE_6B             = 0x6B,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_DMAC0               = 0x6C,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_DMAC1               = 0x6D,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_DIVU_OVFI           = 0x6E,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_FREE_6F             = 0x6F,
        /// Interrupt vector.
        CPU_INTC_INTERRUPT_SLAVE_ENTRY         = 0x94,
} cpu_intc_interrupt_t;

/// @brief Callback type.
/// @see cpu_intc_ihr_set
typedef void (*cpu_intc_ihr)(void);

/// @brief Set the interrupt handler for the specified CPU related interrupt.
///
/// @details This is a BIOS call. The function must use `rte` to return. Use the
/// @ref __interrupt_handler GCC attribute.
///
/// To set a handler in the slave CPU vector table, add the offset @ref
/// CPU_INTC_INTERRUPT_SLAVE_BASE to @p vector.
///
/// @warning Do not use this function to set your interrupt handler, unless
/// there is an explicit need to do so. To see which functions and macros to use
/// for each interrupt, see @ref CPU_INTC_HELPERS.
///
/// @param vector The vector number.
/// @param ihr    The interrupt handler.
static inline void __always_inline
cpu_intc_ihr_set(cpu_intc_interrupt_t vector, cpu_intc_ihr ihr)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000310;

        ((void (*)(uint32_t, void (*)(void)))*bios_address)(vector, ihr);
}

/// @brief Clear the interrupt handler for the specified CPU related interrupt.
///
/// @details This is a BIOS call.
///
/// To clear a handler in the slave CPU vector table, add the offset @ref
/// CPU_INTC_INTERRUPT_SLAVE_BASE to @p vector.
///
/// @warning Do not use this function to clear your interrupt handler, unless
/// there is an explicit need to do so. To see which functions and macros to use
/// for each interrupt, see @ref CPU_INTC_HELPERS.
///
/// @param vector The vector number.
static inline void __always_inline
cpu_intc_ihr_clear(cpu_intc_interrupt_t vector)
{
        cpu_intc_ihr_set(vector, NULL);
}

/// @brief Obtain the interrupt handler for the specified CPU related interrupt.
///
/// @details This is a BIOS call.
///
/// To get a handler in the slave CPU vector table, add the offset @ref
/// CPU_INTC_INTERRUPT_SLAVE_BASE to @p vector.
///
/// @param vector The vector number.
static inline cpu_intc_ihr __always_inline
cpu_intc_ihr_get(cpu_intc_interrupt_t vector)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000314;

        return ((void (*(*)(uint32_t))(void))*bios_address)(vector);
}

/// @brief Obtain the interrupt priority level `I` mask bits from the `sr`
/// register.
/// @returns The interrupt priority level `I` mask bits from the `sr` register.
static inline uint8_t __always_inline
cpu_intc_mask_get(void)
{
        register const uint32_t reg_sr = cpu_reg_sr_get();
        register const uint32_t mask = (reg_sr & CPU_SR_I_BITS_MASK) >> 4;

        return mask;
}

/// @brief Set the interrupt priority level.
///
/// @param mask The interrupt priority level.
static inline void __always_inline
cpu_intc_mask_set(uint8_t mask)
{
        register uint32_t reg_sr;
        reg_sr = cpu_reg_sr_get();

        reg_sr &= ~CPU_SR_I_BITS_MASK;
        reg_sr |= (mask << 4) & CPU_SR_I_BITS_MASK;

        cpu_reg_sr_set(reg_sr);
}

/// @brief Obtain the value of the 2-byte value of the @ref IPRA I/O register.
/// @returns The 2-byte value of the @ref IPRA I/O register.
static inline uint16_t __always_inline
cpu_intc_priority_a_get(void)
{
        return MEMORY_READ(16, CPU(IPRA));
}

/// @brief Obtain the value of the 2-byte value of the @ref IPRB I/O register.
/// @returns The 2-byte value of the @ref IPRB I/O register.
static inline uint16_t __always_inline
cpu_intc_priority_b_get(void)
{
        return MEMORY_READ(16, CPU(IPRB));
}

/// @brief Write a 2-byte value to the @ref IPRA I/O register.
///
/// @param ipra The value to write to.
static inline void __always_inline
cpu_intc_priority_a_set(uint16_t ipra)
{
        MEMORY_WRITE(16, CPU(IPRA), ipra);
}

/// @brief Write a 2-byte value to the @ref IPRB I/O register.
///
/// @param iprb The value to write to.
static inline void __always_inline
cpu_intc_priority_b_set(uint16_t iprb)
{
        MEMORY_WRITE(16, CPU(IPRA), iprb);
}

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_INTC_H_ */
