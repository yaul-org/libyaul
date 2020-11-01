/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

/// @defgroup SCU_TIMER

#ifndef _SCU_TIMER_H_
#define _SCU_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#include <sys/cdefs.h>

#include <scu/map.h>

__BEGIN_DECLS

/// @defgroup SCU_TIMER_INLINE_FUNCTIONS
/// @defgroup SCU_TIMER_FUNCTIONS

typedef void (*scu_timer_ihr)(void);

/// @addtogroup SCU_TIMER_INLINE_FUNCTIONS
/// @{

/// @brief Not yet documented.
static inline void __always_inline
scu_timer_t0_value_set(uint16_t value)
{
        MEMORY_WRITE(32, SCU(T0C), value & 0x03FF);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_timer_t1_value_set(uint16_t value)
{
        MEMORY_WRITE(32, SCU(T1S), value & 0x01FF);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_timer_disable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000000);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_timer_enable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000001);
}

/// @brief Not yet documented.
static inline void __always_inline
scu_timer_line_enable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000101);
}

/// @brief Not yet documented.
#define scu_timer_t0_clear() do {                                              \
        scu_timer_t0_set(NULL);                                                \
} while (false)

/// @brief Not yet documented.
#define scu_timer_t1_clear() do {                                              \
        scu_timer_t1_set(NULL);                                                \
} while (false)

/// @}

/// @addtogroup SCU_TIMER_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void scu_timer_t0_set(scu_timer_ihr);
/// @brief Not yet documented.
extern void scu_timer_t1_set(scu_timer_ihr);
/// @brief Not yet documented.
extern void scu_timer_t1_line_set(int16_t);

/// @}

__END_DECLS

#endif /* !_SCU_TIMER_H_ */
