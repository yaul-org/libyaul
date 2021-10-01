/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_FRT_H_
#define _YAUL_CPU_FRT_H_

#include <stdbool.h>

#include <sys/cdefs.h>

#include <cpu/map.h>

__BEGIN_DECLS

/// @defgroup CPU_FRT CPU Free-Running Timer (FRT)

/// @addtogroup CPU_FRT
/// @{

/// @deprecated To be removed and replaced as an `enum`.
#define CPU_FRT_CLOCK_DIV_8             0x00
/// @deprecated To be removed and replaced as an `enum`.
#define CPU_FRT_CLOCK_DIV_32            0x01
/// @deprecated To be removed and replaced as an `enum`.
#define CPU_FRT_CLOCK_DIV_128           0x02
/// Not yet documented.
#define CPU_FRT_CLOCK_DIV_RISING_EDGE   0x03

/// Tick count value representing 1ms for a NTSC machine in 320-mode with a
/// @f$\frac{\phi}{8}@f$ clock divisor.
#define CPU_FRT_NTSC_320_8_COUNT_1MS    0x0D1F
/// Tick count value representing 1ms for a NTSC machine in 320-mode with a
/// @f$\frac{\phi}{32}@f$ clock divisor.
#define CPU_FRT_NTSC_320_32_COUNT_1MS   0x0348
/// Tick count value representing 1ms for a NTSC machine in 320-mode with a
/// @f$\frac{\phi}{128}@f$ clock divisor.
#define CPU_FRT_NTSC_320_128_COUNT_1MS  0x00D2

/// Tick count value representing 1ms for a NTSC machine in 352-mode with a
/// @f$\frac{\phi}{8}@f$ clock divisor.
#define CPU_FRT_NTSC_352_8_COUNT_1MS    0x0DFC
/// Tick count value representing 1ms for a NTSC machine in 352-mode with a
/// @f$\frac{\phi}{32}@f$ clock divisor.
#define CPU_FRT_NTSC_352_32_COUNT_1MS   0x037F
/// Tick count value representing 1ms for a NTSC machine in 352-mode with a
/// @f$\frac{\phi}{128}@f$ clock divisor.
#define CPU_FRT_NTSC_352_128_COUNT_1MS  0x00E0

/// Tick count value representing 1ms for a PAL machine in 320-mode with a
/// @f$\frac{\phi}{8}@f$ clock divisor.
#define CPU_FRT_PAL_320_8_COUNT_1MS     0x0D08
/// Tick count value representing 1ms for a PAL machine in 320-mode with a
/// @f$\frac{\phi}{32}@f$ clock divisor.
#define CPU_FRT_PAL_320_32_COUNT_1MS    0x0342
/// Tick count value representing 1ms for a PAL machine in 320-mode with a
/// @f$\frac{\phi}{128}@f$ clock divisor.
#define CPU_FRT_PAL_320_128_COUNT_1MS   0x00D0

/// Tick count value representing 1ms for a PAL machine in 352-mode with a
/// @f$\frac{\phi}{8}@f$ clock divisor.
#define CPU_FRT_PAL_352_8_COUNT_1MS     0x0DFC
/// Tick count value representing 1ms for a PAL machine in 352-mode with a
/// @f$\frac{\phi}{32}@f$ clock divisor.
#define CPU_FRT_PAL_352_32_COUNT_1MS    0x037F
/// Tick count value representing 1ms for a PAL machine in 352-mode with a
/// @f$\frac{\phi}{128}@f$ clock divisor.
#define CPU_FRT_PAL_352_128_COUNT_1MS   0x00E0

/// @brief Callback type.
/// @see cpu_frt_oca_set
/// @see cpu_frt_ocb_set
/// @see cpu_frt_ovi_set
typedef void (*cpu_frt_ihr_t)(void);

/// @brief Set the 2-byte FRT tick count.
///
/// @param count The 2-byte tick value to set.
static inline void __always_inline
cpu_frt_count_set(uint16_t count)
{
        MEMORY_WRITE(8, CPU(FRCH), (uint8_t)(count >> 8));
        MEMORY_WRITE(8, CPU(FRCL), (uint8_t)(count & 0xFF));
}

/// @brief Obtain the current 2-byte FRT tick count.
/// @returns Return the 2-byte FRT tick count.
static inline uint16_t __always_inline
cpu_frt_count_get(void)
{
        uint16_t reg_frth;
        reg_frth = MEMORY_READ(8, CPU(FRCH));

        uint16_t reg_frtl;
        reg_frtl = MEMORY_READ(8, CPU(FRCL));

        return (reg_frth << 8) | reg_frtl;
}

/// @brief Obtain the ICR value.
///
/// @details When a rising edge or falling edge of the input capture signal is
/// detected, the current FRT tick count value is transferred to the ICR.
///
/// @returns The ICR value.
static inline uint16_t __always_inline
cpu_frt_input_capture_get(void)
{
        uint8_t reg_ficrh;
        reg_ficrh = MEMORY_READ(8, CPU(FICRH));

        uint8_t reg_ficrl;
        reg_ficrl = MEMORY_READ(8, CPU(FICRL));

        return (reg_ficrh << 8) | reg_ficrl;
}

/// @brief Obtain the interrupt priority level for CPU-FRT.
/// @returns The interrupt priority level ranging from `0` to `15`.
static inline uint8_t __always_inline
cpu_frt_interrupt_priority_get(void)
{
        const uint16_t iprb = MEMORY_READ(16, CPU(IPRB));

        return ((iprb >> 8) & 0x0F);
}

/// @brief Set the interrupt priority level for CPU-FRT.
///
/// @param priority The priority ranging from `0` to `15`.
static inline void __always_inline
cpu_frt_interrupt_priority_set(uint8_t priority)
{
        /* Set the interrupt priority level for FRT (shared amongst all FRT
         * related interrupts */
        MEMORY_WRITE_AND(16, CPU(IPRB), 0xF7FF);
        MEMORY_WRITE_OR(16, CPU(IPRB), (priority & 0x0F) << 8);
}

/// @ingroup CPU_INTC_HELPERS
/// @brief Clear the interrupt handler for the CPU-FRT OCA interrupt.
/// @see cpu_frt_oca_set
#define cpu_frt_oca_clear()                                                    \
do {                                                                           \
        cpu_frt_oca_set(0, NULL);                                              \
} while (false)

/// @ingroup CPU_INTC_HELPERS
/// @brief Clear the interrupt handler for the CPU-FRT OCB interrupt.
/// @see cpu_frt_ocb_set
#define cpu_frt_ocb_clear()                                                    \
do {                                                                           \
        cpu_frt_ocb_set(0, NULL);                                              \
} while (false)

/// @ingroup CPU_INTC_HELPERS
/// @brief Clear the interrupt handler for the CPU-FRT OVI interrupt.
/// @see cpu_frt_ovi_set
#define cpu_frt_ovi_clear()                                                    \
do {                                                                           \
        cpu_frt_ovi_set(NULL);                                                 \
} while (false)

/// @brief Fully initialize the CPU-FRT depending with a specific clock divisor.
///
/// @details This function is heavy handed in that it resets the interrupt
/// priority levels for all FRT related interrupts, clears the interrupt
/// handlers, and resets the FRT count.
///
/// This function will properly initialize the CPU-FRT on either the master or
/// slave CPU.
///
/// @param clock_div The clock divisor.
extern void cpu_frt_init(uint8_t clock_div);

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the interrupt handler for the CPU-FRT OCA interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr   The interrupt handler.
/// @param count The tick count to trigger OCA on.
///
/// @see cpu_frt_oca_clear
extern void cpu_frt_oca_set(uint16_t count, cpu_frt_ihr_t ihr);

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the interrupt handler for the CPU-FRT OCB interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr   The interrupt handler.
/// @param count The tick count to trigger OCB on.
///
/// @see cpu_frt_ocb_clear
extern void cpu_frt_ocb_set(uint16_t count, cpu_frt_ihr_t ihr);

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the interrupt handler for the CPU-FRT OVI interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr The interrupt handler.
///
/// @see cpu_frt_ovi_clear
extern void cpu_frt_ovi_set(cpu_frt_ihr_t ihr);

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_FRT_H_ */
