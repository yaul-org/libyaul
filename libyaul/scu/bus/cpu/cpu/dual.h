/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_DUAL_H_
#define _CPU_DUAL_H_

#include <stdint.h>

#include <cpu/registers.h>

__BEGIN_DECLS

#define CPU_MASTER      0
#define CPU_SLAVE       1

#define CPU_DUAL_ENTRY_POLLING 0
#define CPU_DUAL_ENTRY_ICI     1

static inline void __always_inline
cpu_dual_master_notify(void)
{
        MEMORY_WRITE(16, SINIT, 0xFFFF);
}

static inline void __always_inline
cpu_dual_slave_notify(void)
{
        MEMORY_WRITE(16, MINIT, 0xFFFF);
}

static inline void __always_inline
cpu_dual_notification_wait(void)
{
        volatile uint8_t *reg_ftcsr;
        reg_ftcsr = (volatile uint8_t *)CPU(FTCSR);

        while ((*reg_ftcsr & 0x80) == 0x00);

        *reg_ftcsr &= ~0x80;
}

extern void cpu_dual_init(uint8_t);
extern void cpu_dual_master_set(void (*)(void));
extern void cpu_dual_slave_set(void (*)(void));

#define cpu_dual_master_clear() do {                                           \
        cpu_dual_master_set(NULL);                                             \
} while (false)

#define cpu_dual_slave_clear() do {                                            \
        cpu_dual_slave_set(NULL);                                              \
} while (false)

__END_DECLS

#endif /* !_CPU_DUAL_H */
