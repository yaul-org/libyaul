/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Nikita Sokolov <hitomi2500@mail.ru>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_SCI_H_
#define _YAUL_CPU_SCI_H_

#include <sys/cdefs.h>

#include <cpu/map.h>

__BEGIN_DECLS

/// @defgroup CPU_SCI CPU Serial Communication Interface (SCI)

/// @addtogroup CPU_SCI
/// @{

/// @brief Obtain the interrupt priority level for CPU-SCI.
/// @returns The interrupt priority level ranging from `0` to `15`.
static inline uint8_t __always_inline
cpu_sci_interrupt_priority_get(void)
{
        const uint16_t iprb = MEMORY_READ(16, CPU(IPRB));

        return ((iprb >> 12) & 0x0F);
}

/// @brief Set the interrupt priority level for CPU-SCI.
///
/// @param priority The priority ranging from `0` to `15`.
static inline void __always_inline
cpu_sci_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRB), 0x7FFF);
        MEMORY_WRITE_OR(16, CPU(IPRB), (priority & 0x0F) << 12);
}

/// @brief Stop CPU-SCI.
static inline void __always_inline
cpu_sci_stop(void)
{
        MEMORY_WRITE(8, CPU(SSR), 0x00);
        MEMORY_WRITE(8, CPU(SCR), 0x00);
}

/// @brief Initialize the CPU-SCI.
extern void cpu_sci_init(void);

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_SCI_H_ */
