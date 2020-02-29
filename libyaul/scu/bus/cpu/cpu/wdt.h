/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_WDT_H_
#define _CPU_WDT_H_

__BEGIN_DECLS

/// @defgroup CPU_WDT_DEFINES
/// @defgroup CPU_WDT_INLINE_FUNCTIONS
/// @defgroup CPU_WDT_FUNCTIONS

/// @addtogroup CPU_WDT_DEFINES
/// @{

/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_2     0
/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_64    1
/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_128   2
/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_256   3
/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_512   4
/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_1024  5
/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_4096  6
/// Not yet documented.
#define CPU_WDT_CLOCK_DIV_8192  7

/// Not yet documented.
#define CPU_WDT_TIMER_MODE_INTERVAL 0
/// Not yet documented.
#define CPU_WDT_TIMER_MODE_WATCHDOG 1

/// @}

/// @addtogroup CPU_WDT_INLINE_FUNCTIONS
/// @{

/// @brief Not yet documented.
static inline void __always_inline
cpu_wdt_count_set(uint8_t count)
{
        MEMORY_WRITE_WTCNT(count);
}

/// @brief Not yet documented.
static inline uint8_t __always_inline
cpu_wdt_count_get(void)
{
        return MEMORY_READ(8, CPU(WTCNTR));
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_wdt_enable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0x80;
        wtcr_bits |= 0x20;

        MEMORY_WRITE_WTCSR(wtcr_bits);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_wdt_disable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0xA0;

        MEMORY_WRITE_WTCSR(wtcr_bits);
}

/// @brief Not yet documented.
static inline void __always_inline
cpu_wdt_interrupt_priority_set(uint8_t priority)
{
        uint16_t ipra;
        ipra = MEMORY_READ(16, CPU(IPRA));

        ipra = (ipra & 0xFF0F) | ((priority & 0x0F) << 4);

        MEMORY_WRITE(16, CPU(IPRA), ipra);
}

/// @}

/// @addtogroup CPU_WDT_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void cpu_wdt_init(uint8_t);

/// @brief Not yet documented.
extern void cpu_wdt_timer_mode_set(uint8_t, void (*)(void));

/// @}

__END_DECLS

#endif /* !_CPU_WDT_H_ */
