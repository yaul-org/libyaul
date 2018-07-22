/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_SLAVE_H_
#define _CPU_SLAVE_H_

#include <stdint.h>

#include <cpu/registers.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CPU_MASTER      0
#define CPU_SLAVE       1

static inline void __attribute__ ((always_inline))
cpu_slave_notify(uint8_t cpu)
{
        volatile uint16_t *reg_init;
        reg_init = (volatile uint16_t *)(((cpu & 0x01) == CPU_MASTER)
            ? SINIT
            : MINIT);

        *reg_init = 0xFFFF;
}

static inline void __attribute__ ((always_inline))
cpu_slave_notification_wait(void)
{
        volatile uint8_t *reg_ftcsr;
        reg_ftcsr = (volatile uint8_t *)CPU(FTCSR);

        while ((*reg_ftcsr & 0x80) == 0x00);

        *reg_ftcsr &= ~0x80;
}

extern void cpu_slave_init(void);
extern void cpu_slave_entry_set(uint8_t, void (*)(void));

#define cpu_slave_entry_clear(cpu) do {                                        \
        cpu_slave_entry_set(cpu, NULL);                                        \
} while (false)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_SLAVE_H */
