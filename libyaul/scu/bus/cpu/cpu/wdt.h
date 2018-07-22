/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_WDT_H_
#define _CPU_WDT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define WDT_CLOCK_DIV_2         0
#define WDT_CLOCK_DIV_64        1
#define WDT_CLOCK_DIV_128       2
#define WDT_CLOCK_DIV_256       3
#define WDT_CLOCK_DIV_512       4
#define WDT_CLOCK_DIV_1024      5
#define WDT_CLOCK_DIV_4096      6
#define WDT_CLOCK_DIV_8192      7

static inline void __attribute__ ((always_inline))
cpu_wdt_count_set(uint8_t count)
{
        MEMORY_WRITE(16, CPU(WTCNTW), 0x5A00 | count);
}

static inline uint8_t __attribute__ ((always_inline))
cpu_wdt_count_get(void)
{
        return MEMORY_READ(8, CPU(WTCNTR));
}

static inline void __attribute__ ((always_inline))
cpu_wdt_enable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0x80;
        wtcr_bits |= 0x20;

        MEMORY_WRITE(16, CPU(WTCSRW), 0x5A18 | wtcr_bits);
}

static inline void __attribute__ ((always_inline))
cpu_wdt_disable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0xA0;

        MEMORY_WRITE(16, CPU(WTCSRW), 0x5A18 | wtcr_bits);
}

static inline void __attribute__ ((always_inline))
cpu_wdt_interrupt_priority_set(uint8_t priority)
{
        uint16_t ipra;
        ipra = MEMORY_READ(16, CPU(IPRA));

        ipra = (ipra & 0xFF0F) | ((priority & 0x0F) << 4);

        MEMORY_WRITE(16, CPU(IPRA), ipra);
}

extern void cpu_wdt_init(uint8_t);
extern void cpu_wdt_interval_mode_set(void (*)(void));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_WDT_H_ */
