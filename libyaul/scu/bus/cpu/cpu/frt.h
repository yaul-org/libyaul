/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_FRT_H_
#define _CPU_FRT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpu/map.h>

#define FRT_CLOCK_DIV_8                 0x00
#define FRT_CLOCK_DIV_32                0x01
#define FRT_CLOCK_DIV_128               0x02
#define FRT_CLOCK_DIV_RISING_EDGE       0x03

#define FRT_CLOCK_DIV_8_COUNT_1MS       0x0D1F
#define FRT_CLOCK_DIV_32_COUNT_1MS      0x0348
#define FRT_CLOCK_DIV_128_COUNT_1MS     0x00D2

static inline void __attribute__ ((always_inline))
cpu_frt_count_set(uint16_t count)
{
        MEMORY_WRITE(8, CPU(FRCH), (uint8_t)(count >> 8));
        MEMORY_WRITE(8, CPU(FRCL), (uint8_t)(count & 0xFF));
}

static inline uint16_t __attribute__ ((always_inline))
cpu_frt_count_get(void)
{
        register uint16_t reg_frth;
        reg_frth = MEMORY_READ(8, CPU(FRCH));

        register uint16_t reg_frtl;
        reg_frtl = MEMORY_READ(8, CPU(FRCL));

        return (reg_frth << 8) | reg_frtl;
}

static inline uint8_t __attribute__ ((always_inline))
cpu_frt_status_get(void)
{
        register uint8_t reg_ftcsr;
        reg_ftcsr = MEMORY_READ(8, CPU(FTCSR));

        return reg_ftcsr;
}

static inline void
cpu_frt_ocra_set(uint16_t count)
{
        uint16_t reg_tocr;
        reg_tocr = MEMORY_READ(8, CPU(TOCR));
        /* Select OCRA register and select output compare A match */
        reg_tocr = (reg_tocr & ~0x04) | 0x2;

        MEMORY_WRITE(8, CPU(TOCR), reg_tocr);
        MEMORY_WRITE(8, CPU(OCRAH), (uint8_t)(count >> 8));
        MEMORY_WRITE(8, CPU(OCRAL), (uint8_t)(count & 0xFF));
}

static inline void
cpu_frt_ocrb_set(uint16_t count)
{
        uint16_t reg_tocr;
        reg_tocr = MEMORY_READ(8, CPU(TOCR));
        /* Select OCRB register and select output compare B match */
        reg_tocr = reg_tocr | 0x04 | 0x01;

        MEMORY_WRITE(8, CPU(TOCR), reg_tocr);
        MEMORY_WRITE(8, CPU(OCRBH), (uint8_t)(count >> 8));
        MEMORY_WRITE(8, CPU(OCRBL), (uint8_t)(count & 0xFF));
}

extern void cpu_frt_init(uint8_t);

#endif /* !_CPU_FRT_H_ */
