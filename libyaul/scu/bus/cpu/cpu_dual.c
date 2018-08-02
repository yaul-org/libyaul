/*
 * Copyright (c) 2012-2016
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

static void _slave_init(void);

static void _slave_polling_entry(void);
static void _slave_ici_entry(void);

static void _master_ici_handler(void);
static void _slave_ici_handler(void);

static void _default_entry(void);

static void (*_master_entry)(void) = _default_entry;
static void (*_slave_entry)(void) __section(".uncached") = _default_entry;

static void (*_slave_entry_table[])(void) = {
        &_slave_polling_entry,
        &_slave_ici_entry
};

void
cpu_dual_init(uint8_t entry_type)
{
        uint8_t sr_mask;
        sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        cpu_dual_master_clear();
        cpu_dual_slave_clear();

        cpu_intc_ihr_set(INTC_INTERRUPT_FRT_ICI, _master_ici_handler);

        smpc_smc_sshoff_call();

        cpu_intc_ihr_set(INTC_INTERRUPT_SLAVE_BASE + INTC_INTERRUPT_FRT_ICI,
            _slave_ici_handler);

        cpu_intc_ihr_set(INTC_INTERRUPT_SLAVE_ENTRY,
            _slave_entry_table[entry_type & 0x01]);

        smpc_smc_sshon_call();

        cpu_intc_mask_set(sr_mask);
}

void
cpu_dual_master_set(void (*entry)(void))
{
        volatile uint8_t *reg_tier;
        reg_tier = (volatile uint8_t *)CPU(TIER);

        *reg_tier &= ~0x80;

        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

        _master_entry = _default_entry;

        if (entry != NULL) {
                _master_entry = entry;

                *reg_tier |= 0x80;
        }
}

void
cpu_dual_slave_set(void (*entry)(void))
{
        _slave_entry = _default_entry;

        if (entry != NULL) {
                _slave_entry = entry;
        }
}

static void
_slave_init(void)
{
        cpu_divu_init();
        cpu_frt_init(FRT_CLOCK_DIV_8);
        cpu_wdt_init(WDT_CLOCK_DIV_2);
        cpu_dmac_init();

        cpu_intc_mask_set(0);

        cpu_cache_purge();
}

static void __noreturn __aligned(16)
_slave_polling_entry(void)
{
        _slave_init();

        while (true) {
                cpu_dual_notification_wait();

                _slave_entry();
        }
}

static void __noreturn
_slave_ici_entry(void)
{
        _slave_init();

        MEMORY_WRITE_OR(8, CPU(TIER), 0x80);

        while (true) {
        }
}

static void __attribute__ ((interrupt_handler))
_master_ici_handler(void)
{
        volatile uint8_t *reg_tier;
        reg_tier = (volatile uint8_t *)CPU(TIER);

        *reg_tier &= ~0x80;

        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

        _master_entry();

        *reg_tier |= 0x80;
}

static void __attribute__ ((interrupt_handler))
_slave_ici_handler(void)
{
        volatile uint8_t *reg_tier;
        reg_tier = (volatile uint8_t *)CPU(TIER);

        *reg_tier &= ~0x80;

        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

        _slave_entry();

        *reg_tier |= 0x80;
}

static void
_default_entry(void)
{
}
