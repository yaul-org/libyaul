/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <cpu/cache.h>
#include <cpu/divu.h>
#include <cpu/dmac.h>
#include <cpu/dual.h>
#include <cpu/frt.h>
#include <cpu/instructions.h>
#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/registers.h>
#include <cpu/wdt.h>

#include <smpc/smc.h>

#include <cpu-internal.h>

extern void __slave_polling_entry_trampoline(void);
extern void __slave_ici_entry_trampoline(void);

typedef void (*slave_entry_t)(void);

static void _slave_init(void);

static void _master_ici_handler(void);
static void _slave_ici_handler(void);

static void _default_entry(void);

static cpu_dual_master_entry_t _master_entry = _default_entry;
static cpu_dual_slave_entry_t _slave_entry __uncached = _default_entry;

static slave_entry_t _slave_entry_table[] = {
    &__slave_polling_entry_trampoline,
    &__slave_ici_entry_trampoline
};

void
cpu_dual_comm_mode_set(cpu_dual_comm_mode_t mode)
{
    const uint8_t sr_mask = cpu_intc_mask_get();

    cpu_intc_mask_set(15);

    cpu_dual_master_clear();
    cpu_dual_slave_clear();

    cpu_intc_ihr_set(CPU_INTC_INTERRUPT_FRT_ICI, _master_ici_handler);

    smpc_smc_sshoff_call();

    cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SLAVE_BASE + CPU_INTC_INTERRUPT_FRT_ICI,
      _slave_ici_handler);

    cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SLAVE_ENTRY,
      _slave_entry_table[mode & 0x01]);

    smpc_smc_sshon_call();

    cpu_intc_mask_set(sr_mask);
}

void
cpu_dual_master_set(cpu_dual_master_entry_t entry)
{
    volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

    cpu_ioregs->tier &= ~0x80;
    cpu_ioregs->ftcsr &= ~0x80;

    _master_entry = _default_entry;

    if (entry != NULL) {
        _master_entry = entry;

        cpu_ioregs->tier |= 0x80;
    }
}

void
cpu_dual_slave_set(cpu_dual_slave_entry_t entry)
{
    _slave_entry = _default_entry;

    if (entry != NULL) {
        _slave_entry = entry;
    }
}

void __noreturn __aligned(16) __used
    __slave_polling_entry(void)
{
    _slave_init();

    while (true) {
        cpu_dual_notification_wait();

        _slave_entry();
    }
}

void __noreturn __aligned(16) __used
    __slave_ici_entry(void)
{
    volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

    _slave_init();

    cpu_ioregs->tier |= 0x80;

    while (true) {
    }
}

static void
_slave_init(void)
{
    cpu_intc_mask_set(15);

    __cpu_divu_init();
    cpu_frt_init(CPU_FRT_CLOCK_DIV_8);
    cpu_wdt_init(CPU_WDT_CLOCK_DIV_2);
    __cpu_dmac_init();

    /* SCU interrupts 0x41 (H-BLANK-IN, IRL2) and 0x41 (V-BLANK-IN, IRL6)
     * can only be masked is by setting the priority level to 14.
     *
     * In the case where an interrupt does need to be raised, for example
     * the CPU-FRT ICI interrupt, then its priority level must be set to 15
     * through the CPU(IPRA) and CPU(IPRB) I/O registers */
    cpu_intc_mask_set(14);

    cpu_cache_purge();
}

static void __interrupt_handler
_master_ici_handler(void)
{
    volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

    cpu_ioregs->tier &= ~0x80;
    cpu_ioregs->ftcsr &= ~0x80;

    _master_entry();

    cpu_ioregs->tier |= 0x80;
}

static void __interrupt_handler
_slave_ici_handler(void)
{
    volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

    cpu_ioregs->tier &= ~0x80;
    cpu_ioregs->ftcsr &= ~0x80;

    _slave_entry();

    cpu_ioregs->tier |= 0x80;
}

static void
_default_entry(void)
{
}
