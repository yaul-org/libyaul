/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_FRT_H_
#define _CPU_FRT_H_

__BEGIN_DECLS

#include <stdbool.h>

#include <cpu/map.h>

#define CPU_FRT_CLOCK_DIV_8             0x00
#define CPU_FRT_CLOCK_DIV_32            0x01
#define CPU_FRT_CLOCK_DIV_128           0x02
#define CPU_FRT_CLOCK_DIV_RISING_EDGE   0x03

#define CPU_FRT_NTSC_320_8_COUNT_1MS    0x0D1F
#define CPU_FRT_NTSC_320_32_COUNT_1MS   0x0348
#define CPU_FRT_NTSC_320_128_COUNT_1MS  0x00D2

#define CPU_FRT_NTSC_352_8_COUNT_1MS    0x0DFC
#define CPU_FRT_NTSC_352_32_COUNT_1MS   0x037F
#define CPU_FRT_NTSC_352_128_COUNT_1MS  0x00E0

#define CPU_FRT_PAL_320_8_COUNT_1MS     0x0D08
#define CPU_FRT_PAL_320_32_COUNT_1MS    0x0342
#define CPU_FRT_PAL_320_128_COUNT_1MS   0x00D0

#define CPU_FRT_PAL_352_8_COUNT_1MS     0x0DFC
#define CPU_FRT_PAL_352_32_COUNT_1MS    0x037F
#define CPU_FRT_PAL_352_128_COUNT_1MS   0x00E0

static inline void __always_inline
cpu_frt_count_set(uint16_t count)
{
        MEMORY_WRITE(8, CPU(FRCH), (uint8_t)(count >> 8));
        MEMORY_WRITE(8, CPU(FRCL), (uint8_t)(count & 0xFF));
}

static inline uint16_t __always_inline
cpu_frt_count_get(void)
{
        uint16_t reg_frth;
        reg_frth = MEMORY_READ(8, CPU(FRCH));

        uint16_t reg_frtl;
        reg_frtl = MEMORY_READ(8, CPU(FRCL));

        return (reg_frth << 8) | reg_frtl;
}

static inline uint16_t __always_inline
cpu_frt_input_capture_get(void)
{
        uint8_t reg_ficrh;
        reg_ficrh = MEMORY_READ(8, CPU(FICRH));

        uint8_t reg_ficrl;
        reg_ficrl = MEMORY_READ(8, CPU(FICRL));

        return (reg_ficrh << 8) | reg_ficrl;
}

static inline void __always_inline
cpu_frt_interrupt_priority_set(uint8_t priority)
{
        /* Set the interrupt priority level for FRT (shared amongst all
         * FRT related interrupts */
        uint16_t iprb;
        iprb = MEMORY_READ(16, CPU(IPRB));

        iprb = (iprb & 0x00FF) | ((priority & 0x0F) << 8);

        MEMORY_WRITE(16, CPU(IPRB), iprb);
}

#define cpu_frt_oca_clear() do {                                               \
        cpu_frt_oca_set(0, NULL);                                              \
} while (false)

#define cpu_frt_ocb_clear() do {                                               \
        cpu_frt_ocb_set(0, NULL);                                              \
} while (false)

#define cpu_frt_ovi_clear() do {                                               \
        cpu_frt_ovi_set(NULL);                                                 \
} while (false)

extern void cpu_frt_init(uint8_t);
extern void cpu_frt_oca_set(uint16_t, void (*)(void));
extern void cpu_frt_ocb_set(uint16_t, void (*)(void));
extern void cpu_frt_ovi_set(void (*)(void));

__END_DECLS

#endif /* !_CPU_FRT_H_ */
