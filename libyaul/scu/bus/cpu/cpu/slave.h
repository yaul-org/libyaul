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

static inline void __attribute__ ((always_inline))
cpu_slave_notify(void)
{
        MEMORY_WRITE(16, MINIT, 0xFFFF);
}

static inline void __attribute__ ((always_inline))
cpu_slave_master_notify(void)
{
        MEMORY_WRITE(16, SINIT, 0xFFFF);
}

extern void cpu_slave_init(void);
extern void cpu_slave_entry_set(void (*entry)(void));

#define cpu_slave_entry_clear() do {                                           \
        cpu_slave_entry_set(NULL);                                             \
} while (false)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_SLAVE_H */
