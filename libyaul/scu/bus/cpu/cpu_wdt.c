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
        /* For writing to WTCSRW and WTCNTW, use 16-bit writes only:
         * WTCSRW = 0xA518 | (value & 0xFF)
         * WTCNTW = 0x5A00 | (value & 0xFF)
         *
         * For writing to RSTCSRW, use 16-bit writes only.
         *
         * When clearing WOVF bit:
         * RSTCSRW = 0xA500 | 0x00
         *
         * When writing RSTE and RSTS bits:
         * RSTCSRW = 0x5A00 | (value & 0xFF)
         *
         * For reading, use 8-bit reads on *R registers only.
         */

        MEMORY_WRITE_AND(16, CPU(VCRWDT), ~0x7F00);
        MEMORY_WRITE_OR(16, CPU(VCRWDT), INTC_INTERRUPT_WDT_ITI << 8);

        MEMORY_WRITE(16, CPU(WTCSRW), 0xA518 | (clock_div & 0x07));

        MEMORY_WRITE(16, CPU(RSTCSRW), 0xA500);
        MEMORY_WRITE(16, CPU(RSTCSRW), 0x5A00);

        cpu_intc_ihr_set(INTC_INTERRUPT_WDT_ITI, _wdt_iti_handler);
}

void
cpu_wdt_interval_mode_set(void (*ihr)(void))
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        /* Clear OVF and TME bits */
        wtcr_bits &= ~0xA0;
        /* Set WTIT bit */
        wtcr_bits |= 0x40;

        MEMORY_WRITE(16, CPU(WTCSRW), 0x5A18 | wtcr_bits);

        MEMORY_WRITE(16, CPU(RSTCSRW), 0xA500);
        MEMORY_WRITE(16, CPU(RSTCSRW), 0x5A00);

        _wdt_iti_ihr = _default_ihr;

        if (ihr != NULL) {
                _wdt_iti_ihr = ihr;
        }

        cpu_wdt_count_set(0);
}

static void __attribute__ ((interrupt_handler))
_wdt_iti_handler(void)
{
        uint8_t wtcr_bits;
        wtcr_bits = MEMORY_READ(8, CPU(WTCSRR));

        /* Clear OVF bit */
        wtcr_bits &= ~0x80;

        MEMORY_WRITE(16, CPU(WTCSRW), 0x5A18 | wtcr_bits);

        /* User is responsible for resetting WDT count */
        _wdt_iti_ihr();
}

static void
_default_ihr(void)
{
}
