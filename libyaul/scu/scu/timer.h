/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCU_TIMER_H_
#define _YAUL_SCU_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#include <sys/cdefs.h>

#include <scu/map.h>

__BEGIN_DECLS

/// @defgroup SCU_TIMER SCU Timer
/// Not yet documented.

/// @addtogroup SCU_TIMER
/// @{

/// @brief Callback type.
/// @see scu_timer_t0_set
/// @see scu_timer_t1_set
typedef void (*scu_timer_ihr_t)(void);

/// @brief Set the compare value for SCU timer #0.
///
/// @param value The 2-byte compare value.
static inline void __always_inline
scu_timer_t0_value_set(uint16_t value)
{
        MEMORY_WRITE(32, SCU(T0C), value & 0x03FF);
}

/// @brief Set the compare value for SCU timer #1.
///
/// @param value The 2-byte compare value.
static inline void __always_inline
scu_timer_t1_value_set(uint16_t value)
{
        MEMORY_WRITE(32, SCU(T1S), value & 0x01FF);
}

/// @brief Disable SCU timers.
static inline void __always_inline
scu_timer_disable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000000);
}

/// @brief Enable SCU timers.
static inline void __always_inline
scu_timer_enable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000001);
}

/// @brief Set behavior of the SCU timer #1 interrupt to occur at lines indicated by SCU timer #0
///
/// This function also enables the SCU timers.
///
/// @see scu_timer_line_disable
static inline void __always_inline
scu_timer_line_enable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000101);
}

/// @brief Set behavior of the SCU timer #1 interrupt to occur at every line.
///
/// This function also enables the SCU timers.
///
/// @see scu_timer_line_enable
static inline void __always_inline
scu_timer_line_disable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000001);
}

/// @ingroup SCU_IC_HELPERS
/// @brief Clear the interrupt handler for the SCU timer #0 interrupt.
/// @see scu_timer_t0_set
#define scu_timer_t0_clear()                                                   \
do {                                                                           \
        scu_timer_t0_set(NULL);                                                \
} while (false)

/// @ingroup SCU_IC_HELPERS
/// @brief Clear the interrupt handler for the SCU timer #1 interrupt.
/// @see scu_timer_t1_set
#define scu_timer_t1_clear()                                                   \
do {                                                                           \
        scu_timer_t1_set(NULL);                                                \
} while (false)

/// @ingroup SCU_IC_HELPERS
/// @brief Set the interrupt handler for the SCU timer #0 interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr The interrupt handler.
///
/// @see scu_timer_t0_clear
extern void scu_timer_t0_set(scu_timer_ihr_t ihr);

/// @ingroup SCU_IC_HELPERS
/// @brief Set the interrupt handler for the SCU timer #1 interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr The interrupt handler.
///
/// @see scu_timer_t1_clear
extern void scu_timer_t1_set(scu_timer_ihr_t ihr);

/// @}

__END_DECLS

#endif /* !_YAUL_SCU_TIMER_H_ */
