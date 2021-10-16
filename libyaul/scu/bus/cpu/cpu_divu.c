/*
 * Copyright (c) 2012-2019
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

static cpu_divu_ihr_t _master_divu_ovfi_ihr = _default_ihr;
static cpu_divu_ihr_t _slave_divu_ovfi_ihr = _default_ihr;

static cpu_divu_ihr_t *_divu_ovfi_ihr_get(void);

void
_internal_cpu_divu_init(void)
{
        cpu_divu_ovfi_clear();

        MEMORY_WRITE(32, CPU(VCRDIV), CPU_INTC_INTERRUPT_DIVU_OVFI);

        const uint8_t which_cpu = cpu_dual_executor_get();

        if (which_cpu == CPU_MASTER) {
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DIVU_OVFI, _divu_ovfi_handler);

                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DIVU_OVFI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _divu_ovfi_handler);
        }
}

void
cpu_divu_ovfi_set(cpu_divu_ihr_t ihr)
{
        volatile uint32_t *reg_dvcr;
        reg_dvcr = (volatile uint32_t *)CPU(DVCR);

        *reg_dvcr &= ~0x00000003;

        cpu_divu_ihr_t *divu_ovfi_ihr;
        divu_ovfi_ihr = _divu_ovfi_ihr_get();

        *divu_ovfi_ihr = _default_ihr;

        if (ihr != NULL) {
                *divu_ovfi_ihr = ihr;

                *reg_dvcr |= 0x00000002;
        }
}

static void __interrupt_handler
_divu_ovfi_handler(void)
{
        MEMORY_WRITE_AND(32, CPU(DVCR), ~0x00000001);

        cpu_divu_ihr_t *divu_ovfi_ihr;
        divu_ovfi_ihr = _divu_ovfi_ihr_get();

        (*divu_ovfi_ihr)();
}

static cpu_divu_ihr_t *
_divu_ovfi_ihr_get(void)
{
        const uint8_t which_cpu = cpu_dual_executor_get();

        switch (which_cpu) {
                case CPU_MASTER:
                        return &_master_divu_ovfi_ihr;
                case CPU_SLAVE:
                        return &_slave_divu_ovfi_ihr;
        }

        return NULL;
}

static void
_default_ihr(void)
{
}
