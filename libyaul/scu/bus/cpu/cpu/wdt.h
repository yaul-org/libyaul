/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_WDT_H_
#define _YAUL_CPU_WDT_H_

__BEGIN_DECLS

/// @defgroup CPU_WDT CPU Watchdog Timer (WDT)

/// @addtogroup CPU_WDT
/// @{

/// @brief CPU-WDT clock divisors.
/// @see cpu_wdt_init
typedef enum cpu_wdt_clock {
        /// WDT clock divisor of @f$\frac{\phi}{2}@f$.
        CPU_WDT_CLOCK_DIV_2    = 0,
        /// WDT clock divisor of @f$\frac{\phi}{64}@f$.
        CPU_WDT_CLOCK_DIV_64   = 1,
        /// WDT clock divisor of @f$\frac{\phi}{128}@f$.
        CPU_WDT_CLOCK_DIV_128  = 2,
        /// WDT clock divisor of @f$\frac{\phi}{256}@f$.
        CPU_WDT_CLOCK_DIV_256  = 3,
        /// WDT clock divisor of @f$\frac{\phi}{512}@f$.
        CPU_WDT_CLOCK_DIV_512  = 4,
        /// WDT clock divisor of @f$\frac{\phi}{1024}@f$.
        CPU_WDT_CLOCK_DIV_1024 = 5,
        /// WDT clock divisor of @f$\frac{\phi}{4096}@f$.
        CPU_WDT_CLOCK_DIV_4096 = 6,
        /// WDT clock divisor of @f$\frac{\phi}{8192}@f$.
        CPU_WDT_CLOCK_DIV_8192 = 7
} cpu_wdt_clock_t;

/// @brief CPU-WDT modes.
/// @see cpu_wdt_timer_mode_set
typedef enum cpu_wdt_mode {
        /// Interval mode.
        CPU_WDT_MODE_INTERVAL = 0,
        /// Watchdog mode.
        CPU_WDT_MODE_WATCHDOG = 1
} cpu_wdt_mode_t;

/// @brief Callback type.
/// @see cpu_wdt_timer_mode_set
typedef void (*cpu_wdt_ihr)(void);

/// @brief Set the 1-byte WDT tick count.
///
/// @param count The 1-byte tick value to set.
static inline void __always_inline
cpu_wdt_count_set(uint8_t count)
{
        MEMORY_WRITE_WTCNT(count);
}

/// @brief Obtain the current 1-byte WDT tick count.
/// @returns Return the 1-byte WDT tick count.
static inline uint8_t __always_inline
cpu_wdt_count_get(void)
{
        return MEMORY_READ(8, CPU(WTCNTR));
}

/// @brief Enable the CPU-WDT.
static inline void __always_inline
cpu_wdt_enable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0x80;
        wtcr_bits |= 0x20;

        MEMORY_WRITE_WTCSR(wtcr_bits);
}

/// @brief Disable the CPU-WDT.
static inline void __always_inline
cpu_wdt_disable(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        wtcr_bits &= ~0xA0;

        MEMORY_WRITE_WTCSR(wtcr_bits);
}

/// @brief Obtain the interrupt priority level for CPU-WDT.
/// @returns The interrupt priority level ranging from `0` to `15`.
static inline uint8_t __always_inline
cpu_wdt_interrupt_priority_get(void)
{
        uint16_t ipra;
        ipra = MEMORY_READ(16, CPU(IPRA));

        return ((ipra >> 4) & 0x0F);
}

/// @brief Set the interrupt priority level for CPU-WDT.
///
/// @param priority The priority ranging from `0` to `15`.
static inline void __always_inline
cpu_wdt_interrupt_priority_set(uint8_t priority)
{
        MEMORY_WRITE_AND(16, CPU(IPRA), 0xFF7F);
        MEMORY_WRITE_OR(16, CPU(IPRA), (priority & 0x0F) << 4);
}

/// @brief Fully initialize the CPU-WDT depending with a specific clock divisor.
///
/// @param clock_div The clock divisor.
extern void cpu_wdt_init(cpu_wdt_clock_t clock_div);

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the CPU-WDT mode.
///
/// @param mode The mode.
/// @param ihr  The interrupt handler.
extern void cpu_wdt_timer_mode_set(cpu_wdt_mode_t mode, cpu_wdt_ihr ihr);

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_WDT_H_ */
