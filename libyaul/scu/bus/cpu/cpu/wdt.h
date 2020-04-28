/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_WDT_H_
#define _CPU_WDT_H_

__BEGIN_DECLS

#define CPU_WDT_CLOCK_DIV_2     0
#define CPU_WDT_CLOCK_DIV_64    1
#define CPU_WDT_CLOCK_DIV_128   2
#define CPU_WDT_CLOCK_DIV_256   3
#define CPU_WDT_CLOCK_DIV_512   4
#define CPU_WDT_CLOCK_DIV_1024  5
#define CPU_WDT_CLOCK_DIV_4096  6
#define CPU_WDT_CLOCK_DIV_8192  7

#define CPU_WDT_TIMER_MODE_INTERVAL 0
#define CPU_WDT_TIMER_MODE_WATCHDOG 1

typedef void (*cpu_wdt_ihr)(void);

static inline void __always_inline
cpu_wdt_count_set(uint8_t count)
{
        MEMORY_WRITE_WTCNT(count);
}

static inline uint8_t __always_inline
cpu_wdt_count_get(void)
{
        return MEMORY_READ(8, CPU(WTCNTR));
}

static inline void __always_inline
cpu_wdt_enable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0x80;
        wtcr_bits |= 0x20;

        MEMORY_WRITE_WTCSR(wtcr_bits);
}

static inline void __always_inline
cpu_wdt_disable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0xA0;

        MEMORY_WRITE_WTCSR(wtcr_bits);
}

static inline uint8_t __always_inline
cpu_wdt_interrupt_priority_get(void)
{
        uint16_t ipra;
        ipra = MEMORY_READ(16, CPU(IPRA));

        return ((ipra >> 4) & 0x0F);
}

static inline void __always_inline
cpu_wdt_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), 0xFF7F);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 4);
}

extern void cpu_wdt_init(uint8_t);
extern void cpu_wdt_timer_mode_set(uint8_t, cpu_wdt_ihr);

__END_DECLS

#endif /* !_CPU_WDT_H_ */
