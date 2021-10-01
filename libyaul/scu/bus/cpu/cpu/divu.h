/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_DIVU_H_
#define _YAUL_CPU_DIVU_H_

#include <cpu/instructions.h>
#include <cpu/map.h>

#include <fix16.h>

__BEGIN_DECLS

/// @defgroup CPU_DIVU CPU Division Unit (DIVU)

/// @addtogroup CPU_DIVU
/// @{

/// @brief Callback type.
/// @see cpu_divu_ovfi_set
typedef void (*cpu_divu_ihr)(void);

/// @brief Determine if an overflow has occurred.
/// @returns `true` if an overflow has occurred. Otherwise, `false`.
static inline bool __always_inline
cpu_divu_status_get(void)
{
        return ((MEMORY_READ(32, CPU(DVCR)) & 0x00000001) == 0x00000001);
}

/// @brief Obtain the quotient part at the end of a division operation.
/// @returns The quotient.
static inline uint32_t __always_inline
cpu_divu_quotient_get(void)
{
        return MEMORY_READ(32, CPU(DVDNTL));
}

/// @brief Obtain the remainder part at the end of a division operation.
/// @returns The remainder.
static inline uint32_t __always_inline
cpu_divu_remainder_get(void)
{
        return MEMORY_READ(32, CPU(DVDNTH));
}

/// @brief Perform a 64÷32 bit division operation.
///
/// @details The results are fetched using @ref cpu_divu_quotient_get.
///
/// @param dividendh, dividendl The dividend.
/// @param divisor              The divisor.
static inline void __always_inline
cpu_divu_64_32_set(uint32_t dividendh, uint32_t dividendl, uint32_t divisor)
{
        MEMORY_WRITE(32, CPU(DVSR), divisor);
        MEMORY_WRITE(32, CPU(DVDNTH), dividendh);
        /* Writing to CPU(DVDNTL) starts the operation */
        MEMORY_WRITE(32, CPU(DVDNTL), dividendl);
}

/// @brief Perform a 32÷32 bit division operation.
///
/// @details The results are fetched using @ref cpu_divu_quotient_get.
///
/// @param dividend The dividend.
/// @param divisor  The divisor.
static inline void __always_inline
cpu_divu_32_32_set(uint32_t dividend, uint32_t divisor)
{
        MEMORY_WRITE(32, CPU(DVSR), divisor);
        /* Writing to CPU(DVDNT) starts the operation */
        MEMORY_WRITE(32, CPU(DVDNT), dividend);
}

/// @brief Split a @ref fix16_t value into two 32-bit parts of the dividend.
///
/// @details Both @p dh and @p dl must not be `NULL`.
///
/// @param[in]  dividend The dividend to be split.
/// @param[out] dh, dl   The upper and lower 32-bits of the dividend, respectively.
static inline void __always_inline
cpu_divu_fix16_split(fix16_t dividend, uint32_t *dh, uint32_t *dl)
{
        *dh = cpu_instr_swapw(dividend);
        *dh = cpu_instr_extsw(*dh);
        *dl = dividend << 16;
}

/// @brief Perform a fixed-point bit division operation.
///
/// @details The results are fetched using @ref cpu_divu_quotient_get. Cast to
/// @ref fix16_t.
///
/// @param dividend The dividend.
/// @param divisor  The divisor.
static inline void __always_inline
cpu_divu_fix16_set(fix16_t dividend, fix16_t divisor)
{
        uint32_t dh;
        uint32_t dl;

        cpu_divu_fix16_split(dividend, &dh, &dl);
        cpu_divu_64_32_set(dh, dl, divisor);
}

/// @brief Obtain the interrupt priority level for CPU-DIVU.
/// @returns The interrupt priority level ranging from `0` to `15`.
static inline uint8_t __always_inline
cpu_divu_interrupt_priority_get(void)
{
        uint16_t ipra = MEMORY_READ(16, CPU(IPRA));

        return ((ipra >> 12) & 0x0F);
}

/// @brief Set the interrupt priority level for CPU-DIVU.
///
/// @param priority The priority ranging from `0` to `15`.
static inline void __always_inline
cpu_divu_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), 0x7FFF);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 12);
}

/// @ingroup CPU_INTC_HELPERS
/// @brief Clear the interrupt handler for the CPU-DIVU OVFI interrupt.
/// @see cpu_divu_ovfi_set
#define cpu_divu_ovfi_clear()                                                  \
do {                                                                           \
        cpu_divu_ovfi_set(NULL);                                               \
} while (false)

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the interrupt handler for the CPU-DIVU OVFI interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr The interrupt handler.
///
/// @see cpu_divu_ovfi_clear
extern void cpu_divu_ovfi_set(cpu_divu_ihr ihr);

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_DIVU_H_ */
