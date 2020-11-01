/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_FRT_H_
#define _CPU_FRT_H_

#include <stdbool.h>

#include <sys/cdefs.h>

#include <cpu/map.h>

__BEGIN_DECLS

/// @defgroup CPU_FRT_HELPERS
/// @defgroup CPU_FRT_FUNCTIONS

/// @addtogroup CPU_FRT_DEFINES
/// @{

/// Not yet documented.
#define CPU_FRT_CLOCK_DIV_8             0x00
/// Not yet documented.
#define CPU_FRT_CLOCK_DIV_32            0x01
/// Not yet documented.
#define CPU_FRT_CLOCK_DIV_128           0x02
/// Not yet documented.
#define CPU_FRT_CLOCK_DIV_RISING_EDGE   0x03

/// Not yet documented.
#define CPU_FRT_NTSC_320_8_COUNT_1MS    0x0D1F
/// Not yet documented.
#define CPU_FRT_NTSC_320_32_COUNT_1MS   0x0348
/// Not yet documented.
#define CPU_FRT_NTSC_320_128_COUNT_1MS  0x00D2

/// Not yet documented.
#define CPU_FRT_NTSC_352_8_COUNT_1MS    0x0DFC
/// Not yet documented.
#define CPU_FRT_NTSC_352_32_COUNT_1MS   0x037F
/// Not yet documented.
#define CPU_FRT_NTSC_352_128_COUNT_1MS  0x00E0

/// Not yet documented.
#define CPU_FRT_PAL_320_8_COUNT_1MS     0x0D08
/// Not yet documented.
#define CPU_FRT_PAL_320_32_COUNT_1MS    0x0342
/// Not yet documented.
#define CPU_FRT_PAL_320_128_COUNT_1MS   0x00D0

/// Not yet documented.
#define CPU_FRT_PAL_352_8_COUNT_1MS     0x0DFC
/// Not yet documented.
#define CPU_FRT_PAL_352_32_COUNT_1MS    0x037F
/// Not yet documented.
#define CPU_FRT_PAL_352_128_COUNT_1MS   0x00E0

/// @}

typedef void (*cpu_frt_ihr)(void);

/// @{
/// @addtogroup CPU_FRT_INLINE_FUNCTIONS

/// @brief Not yet documented.
static inline void __always_inline
cpu_frt_count_set(uint16_t count)
{
        MEMORY_WRITE(8, CPU(FRCH), (uint8_t)(count >> 8));
        MEMORY_WRITE(8, CPU(FRCL), (uint8_t)(count & 0xFF));
}

/// @brief Not yet documented.
static inline uint16_t __always_inline
cpu_frt_count_get(void)
{
        uint16_t reg_frth;
        reg_frth = MEMORY_READ(8, CPU(FRCH));

        uint16_t reg_frtl;
        reg_frtl = MEMORY_READ(8, CPU(FRCL));

        return (reg_frth << 8) | reg_frtl;
}

/// @brief Not yet documented.
static inline uint16_t __always_inline
cpu_frt_input_capture_get(void)
{
        uint8_t reg_ficrh;
        reg_ficrh = MEMORY_READ(8, CPU(FICRH));

        uint8_t reg_ficrl;
        reg_ficrl = MEMORY_READ(8, CPU(FICRL));

        return (reg_ficrh << 8) | reg_ficrl;
}

/// @brief Not yet documented.
static inline uint8_t __always_inline
cpu_frt_interrupt_priority_get(void)
{
        uint16_t iprb;
        iprb = MEMORY_READ(16, CPU(IPRB));

        return ((iprb >> 8) & 0x0F);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_frt_interrupt_priority_set(uint8_t priority)
{
        /* Set the interrupt priority level for FRT (shared amongst all FRT
         * related interrupts */
        MEMORY_WRITE_AND(16, CPU(IPRB), 0xF7FF);
        MEMORY_WRITE_OR(16, CPU(IPRB), (priority & 0x0F) << 8);
}

/// @}

/// @addtogroup CPU_FRT_HELPERS
/// @{

/// @brief Not yet documented.
#define cpu_frt_oca_clear() do {                                               \
        cpu_frt_oca_set(0, NULL);                                              \
} while (false)

/// @brief Not yet documented.
#define cpu_frt_ocb_clear() do {                                               \
        cpu_frt_ocb_set(0, NULL);                                              \
} while (false)

/// @brief Not yet documented.
#define cpu_frt_ovi_clear() do {                                               \
        cpu_frt_ovi_set(NULL);                                                 \
} while (false)

/// @}

/// @addtogroup CPU_FRT_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void cpu_frt_init(uint8_t);
/// @brief Not yet documented.
extern void cpu_frt_oca_set(uint16_t, cpu_frt_ihr);
/// @brief Not yet documented.
extern void cpu_frt_ocb_set(uint16_t, cpu_frt_ihr);
/// @brief Not yet documented.
extern void cpu_frt_ovi_set(cpu_frt_ihr);

/// @}

__END_DECLS

#endif /* !_CPU_FRT_H_ */
