/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <stdio.h>

#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/wdt.h>

static void _wdt_iti_handler(void);

static void _default_ihr(void);

static cpu_wdt_ihr_t _master_wdt_iti_ihr = _default_ihr;
static cpu_wdt_ihr_t _slave_wdt_iti_ihr = _default_ihr;

static cpu_wdt_ihr_t *_wdt_executor_iti_ihr_get(void);

void
cpu_wdt_init(cpu_wdt_clock_t clock_div)
{
        MEMORY_WRITE_AND(16, CPU(VCRWDT), ~0x7F00);
        MEMORY_WRITE_OR(16, CPU(VCRWDT), CPU_INTC_INTERRUPT_WDT_ITI << 8);

        MEMORY_WRITE_WTCSR(clock_div & 0x07);

        MEMORY_CLEAR_WOVF_RSTCSR();
        MEMORY_CLEAR_RSTCSR(0x00);

        const uint8_t which_cpu = cpu_dual_executor_get();

        if (which_cpu == CPU_MASTER) {
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_WDT_ITI, _wdt_iti_handler);

                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_WDT_ITI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _wdt_iti_handler);
        }
}

void
cpu_wdt_timer_mode_set(cpu_wdt_mode_t mode, cpu_wdt_ihr_t ihr)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        /* Clear OVF and TME bits */
        wtcr_bits &= ~0xA0;
        /* Set WTIT bit (timer mode) if necessary */
        wtcr_bits |= (mode & 0x1) << 4;

        MEMORY_WRITE_WTCSR(wtcr_bits);

        MEMORY_CLEAR_WOVF_RSTCSR();
        MEMORY_CLEAR_RSTCSR(0x00);

        cpu_wdt_ihr_t *wdt_iti_ihr;
        wdt_iti_ihr = _wdt_executor_iti_ihr_get();

        *wdt_iti_ihr = _default_ihr;

        if (ihr != NULL) {
                *wdt_iti_ihr = ihr;
        }

        cpu_wdt_count_set(0);
}

static cpu_wdt_ihr_t *
_wdt_executor_iti_ihr_get(void)
{
        const uint8_t which_cpu = cpu_dual_executor_get();

        switch (which_cpu) {
                case CPU_MASTER:
                        return &_master_wdt_iti_ihr;
                case CPU_SLAVE:
                        return &_slave_wdt_iti_ihr;
        }

        return NULL;
}

static void __interrupt_handler
_wdt_iti_handler(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        /* Clear OVF bit */
        wtcr_bits &= ~0x80;

        MEMORY_WRITE_WTCSR(wtcr_bits);

        /* Reset RSTE bit when WTCNT overflows */
        /* MEMORY_CLEAR_RSTCSR(0x40); */

        /* User is responsible for resetting WDT count */
        cpu_wdt_ihr_t *wdt_iti_ihr;
        wdt_iti_ihr = _wdt_executor_iti_ihr_get();

        (*wdt_iti_ihr)();
}

static void
_default_ihr(void)
{
}
