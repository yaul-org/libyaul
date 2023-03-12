/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <smpc/smc.h>

#include <bios-internal.h>

#include <cpu/divu.h>
#include <cpu/frt.h>
#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/which.h>

static void _master_ovfi_handler(void);
static void _slave_ovfi_handler(void);

static void _ovfi_handler(cpu_divu_ihr_t ovfi_ihr);

static cpu_divu_ihr_t _ovfi_ihr_table[] = {
    __BIOS_DEFAULT_HANDLER,
    __BIOS_DEFAULT_HANDLER
};

static cpu_divu_ihr_t *_ovfi_ihr_get(void);

void
__cpu_divu_init(void)
{
    volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

    cpu_divu_ovfi_clear();

    cpu_ioregs->vcrdiv = CPU_INTC_INTERRUPT_DIVU_OVFI;

    const cpu_which_t which_cpu = cpu_dual_executor_get();

    if (which_cpu == CPU_MASTER) {
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DIVU_OVFI, _master_ovfi_handler);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DIVU_OVFI + CPU_INTC_INTERRUPT_SLAVE_BASE,
          _slave_ovfi_handler);
    }
}

void
cpu_divu_ovfi_set(cpu_divu_ihr_t ihr)
{
    volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

    cpu_ioregs->dvcr &= ~0x00000003;

    cpu_divu_ihr_t * const ovfi_ihr = _ovfi_ihr_get();

    *ovfi_ihr = __BIOS_DEFAULT_HANDLER;

    if (ihr != NULL) {
        *ovfi_ihr = ihr;

        cpu_ioregs->dvcr |= 0x00000002;
    }
}

static void __interrupt_handler
_master_ovfi_handler(void)
{
    _ovfi_handler(_ovfi_ihr_table[CPU_MASTER]);
}

static void __interrupt_handler
_slave_ovfi_handler(void)
{
    _ovfi_handler(_ovfi_ihr_table[CPU_SLAVE]);
}

static void
_ovfi_handler(cpu_divu_ihr_t ovfi_ihr)
{
    volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

    cpu_ioregs->dvcr &= ~0x00000001;

    ovfi_ihr();
}

static cpu_divu_ihr_t *
_ovfi_ihr_get(void)
{
    const cpu_which_t which_cpu = cpu_dual_executor_get();

    return &_ovfi_ihr_table[which_cpu];
}
