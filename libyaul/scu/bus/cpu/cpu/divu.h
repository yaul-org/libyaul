/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_DIVU_H_
#define _CPU_DIVU_H_

#include <cpu/instructions.h>
#include <cpu/map.h>

#include <fix16.h>

__BEGIN_DECLS

/// @defgroup CPU_DIVU_INLINE_FUNCTIONS
/// @defgroup CPU_DIVU_HELPERS
/// @defgroup CPU_DIVU_FUNCTIONS

/// @addtogroup CPU_DIVU_INLINE_FUNCTIONS
/// @{

/// @brief Not yet documented.
typedef void (*cpu_divu_ihr)(void);

static inline bool __always_inline
cpu_divu_status_get(void)
{
        return MEMORY_READ(32, CPU(DVCR)) & 0x00000001;
}

/// @brief Not yet documented.
static inline uint32_t __always_inline
cpu_divu_quotient_get(void)
{
        return MEMORY_READ(32, CPU(DVDNTL));
}

/// @brief Not yet documented.
static inline uint32_t __always_inline
cpu_divu_remainder_get(void)
{
        return MEMORY_READ(32, CPU(DVDNTH));
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_divu_64_32_set(uint32_t dividendh, uint32_t dividendl, uint32_t divisor)
{
        MEMORY_WRITE(32, CPU(DVSR), divisor);
        MEMORY_WRITE(32, CPU(DVDNTH), dividendh);
        /* Writing to CPU(DVDNTL) starts the operation */
        MEMORY_WRITE(32, CPU(DVDNTL), dividendl);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_divu_32_32_set(uint32_t dividend, uint32_t divisor)
{
        MEMORY_WRITE(32, CPU(DVSR), divisor);
        /* Writing to CPU(DVDNT) starts the operation */
        MEMORY_WRITE(32, CPU(DVDNT), dividend);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_divu_fix16_set(fix16_t dividend, fix16_t divisor)
{
        uint32_t dh;
        dh = cpu_instr_swapw(dividend);
        dh = cpu_instr_extsw(dh);

        uint32_t dl;
        dl = dividend << 16;

        cpu_divu_64_32_set(dh, dl, divisor);
}

/// @brief Not yet documented.
static inline uint8_t __always_inline
cpu_divu_interrupt_priority_get(void)
{
        uint16_t ipra = MEMORY_READ(16, CPU(IPRA));

        return ((ipra >> 12) & 0x0F);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_divu_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), 0x7FFF);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 12);
}

/// @}

/// @addtogroup CPU_DIVU_HELPERS
/// @{

/// @brief Not yet documented.
#define cpu_divu_ovfi_clear() do {                                             \
        cpu_divu_ovfi_set(NULL);                                               \
} while (false)

/// @}

/// @addtogroup CPU_DIVU_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void cpu_divu_ovfi_set(cpu_divu_ihr);

/// @}

__END_DECLS

#endif /* !_CPU_DIVU_H_ */
