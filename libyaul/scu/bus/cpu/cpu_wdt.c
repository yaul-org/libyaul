/*
 * Copyright (c) 2012-2016
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

static void (*_wdt_iti_ihr)(void) = _default_ihr;
void
cpu_wdt_init(uint8_t clock_div)
{
        MEMORY_WRITE_AND(16, CPU(VCRWDT), ~0x7F00);
        MEMORY_WRITE_OR(16, CPU(VCRWDT), INTC_INTERRUPT_WDT_ITI << 8);

        MEMORY_WRITE_WTCSR(clock_div & 0x07);

        MEMORY_CLEAR_WOVF_RSTCSR();
        MEMORY_CLEAR_RSTCSR(0x00);

        cpu_intc_ihr_set(INTC_INTERRUPT_WDT_ITI, _wdt_iti_handler);
}

void
cpu_wdt_timer_mode_set(uint8_t mode, void (*ihr)(void))
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

        _wdt_iti_ihr = _default_ihr;

        if (ihr != NULL) {
                _wdt_iti_ihr = ihr;
        }

        cpu_wdt_count_set(0);
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
        _wdt_iti_ihr();
}

static void
_default_ihr(void)
{
}
