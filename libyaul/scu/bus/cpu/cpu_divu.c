/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <smpc/smc.h>

#include <cpu/divu.h>
#include <cpu/frt.h>
#include <cpu/intc.h>
#include <cpu/map.h>

static void _divu_ovfi_handler(void);

static void _default_ihr(void);

static void (*_divu_ovfi_ihr)(void) = _default_ihr;

void
cpu_divu_init(void)
{
        cpu_divu_ovfi_clear();

        MEMORY_WRITE(32, CPU(VCRDIV), CPU_INTC_INTERRUPT_DIVU_OVFI);

        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DIVU_OVFI, _divu_ovfi_handler);
}

void
cpu_divu_ovfi_set(void (*ihr)(void))
{
        volatile uint32_t *reg_dvcr;
        reg_dvcr = (volatile uint32_t *)CPU(DVCR);

        *reg_dvcr &= ~0x00000003;

        _divu_ovfi_ihr = _default_ihr;

        if (ihr != NULL) {
                _divu_ovfi_ihr = ihr;

                *reg_dvcr |= 0x00000002;
        }
}

static void __interrupt_handler
_divu_ovfi_handler(void)
{
        MEMORY_WRITE_AND(32, CPU(DVCR), ~0x00000001);

        _divu_ovfi_ihr();
}

static void
_default_ihr(void)
{
}
