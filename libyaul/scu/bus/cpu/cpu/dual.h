/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_DUAL_H_
#define _CPU_DUAL_H_

#include <stdint.h>

#include <scu/map.h>
#include <cpu/frt.h>
#include <cpu/registers.h>

__BEGIN_DECLS

/// @defgroup CPU_DUAL CPU Dual
/// Not yet documented.

/// @addtogroup CPU_DUAL
/// @{


/// Not yet documented.
#define CPU_MASTER      0
/// Not yet documented.
#define CPU_SLAVE       1

/// Not yet documented.
#define CPU_DUAL_ENTRY_POLLING 0
/// Not yet documented.
#define CPU_DUAL_ENTRY_ICI     1

typedef void (*cpu_dual_master_entry)(void);
typedef void (*cpu_dual_slave_entry)(void);

/// @brief Not yet documented.
static inline void __always_inline
cpu_dual_master_notify(void)
{
        MEMORY_WRITE(16, SINIT, 0xFFFF);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dual_slave_notify(void)
{
        MEMORY_WRITE(16, MINIT, 0xFFFF);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_dual_notification_wait(void)
{
        volatile uint8_t *reg_ftcsr;
        reg_ftcsr = (volatile uint8_t *)CPU(FTCSR);

        while ((*reg_ftcsr & 0x80) == 0x00);

        *reg_ftcsr &= ~0x80;
}

/// @brief Not yet documented.
static inline void * __always_inline
cpu_dual_master_stack_get(void)
{
        extern uint32_t _master_stack;

        return (void *)&_master_stack;
}

/// @brief Not yet documented.
static inline void * __always_inline
cpu_dual_slave_stack_get(void)
{
        extern uint32_t _slave_stack;

        return (void *)&_slave_stack;
}

/// @}

/// @brief Not yet documented.
extern void cpu_dual_init(uint8_t);

/// @brief Not yet documented.
extern void cpu_dual_master_set(cpu_dual_master_entry);

/// @brief Not yet documented.
extern void cpu_dual_slave_set(cpu_dual_master_entry);

/// @brief Not yet documented.
#define cpu_dual_master_clear() do {                                           \
        cpu_dual_master_set(NULL);                                             \
} while (false)

/// @brief Not yet documented.
#define cpu_dual_slave_clear() do {                                            \
        cpu_dual_slave_set(NULL);                                              \
} while (false)

/// @brief Not yet documented.
extern void cpu_dual_init(uint8_t);

/// @brief Not yet documented.
extern void cpu_dual_master_set(void (*)(void));

/// @brief Not yet documented.
extern void cpu_dual_slave_set(void (*)(void));

/// @brief Not yet documented.
extern int8_t cpu_dual_executor_get(void);

/// @}

__END_DECLS

#endif /* !_CPU_DUAL_H */
