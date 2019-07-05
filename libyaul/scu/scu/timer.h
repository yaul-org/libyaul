/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_TIMER_H_
#define _SCU_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#include <scu/map.h>

__BEGIN_DECLS

static inline void __always_inline
scu_timer_t0_value_set(uint16_t value)
{
        MEMORY_WRITE(32, SCU(T0C), value & 0x03FF);
}

static inline void __always_inline
scu_timer_t1_value_set(uint16_t value)
{
        MEMORY_WRITE(32, SCU(T1S), value & 0x01FF);
}

static inline void __always_inline
scu_timer_disable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000000);
}

static inline void __always_inline
scu_timer_enable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000001);
}

static inline void __always_inline
scu_timer_line_enable(void)
{
        MEMORY_WRITE(32, SCU(T1MD), 0x00000101);
}

#define scu_timer_t0_clear() do {                                              \
        scu_timer_t0_set(NULL);                                                \
} while (false)

#define scu_timer_t1_clear() do {                                              \
        scu_timer_t1_set(NULL);                                                \
} while (false)

extern void scu_timer_init(void);
extern void scu_timer_t0_set(void (*)(void));
extern void scu_timer_t1_set(void (*)(void));
extern void scu_timer_t1_line_set(int16_t);

__END_DECLS

#endif /* !_SCU_TIMER_H_ */
