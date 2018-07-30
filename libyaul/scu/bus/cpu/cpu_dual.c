/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>
#include <smpc/smc.h>

#include <cpu/cache.h>
#include <cpu/dual.h>
#include <cpu/frt.h>
#include <cpu/instructions.h>
#include <cpu/intc.h>
#include <cpu/map.h>

static void _slave_polling_entry(void);
static void _slave_ici_entry(void);

static void _master_ici_handler(void);
static void _slave_ici_handler(void);

static void _default_entry(void);

static void (*_master_entry)(void) = _default_entry;
static void (*_slave_entry)(void) = _default_entry;

static int8_t _slave_entry_type = -1;

void
cpu_dual_init(void)
{
        _slave_entry_type = -1;

        cpu_dual_master_clear();
        cpu_dual_slave_clear(CPU_DUAL_ENTRY_POLLING);

        cpu_intc_ihr_set(INTC_INTERRUPT_FRT_ICI, _master_ici_handler);
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
cpu_dual_slave_set(uint8_t entry_type, void (*entry)(void))
{
        entry_type &= 0x01;

        if ((_slave_entry_type >= 0) && (entry_type == _slave_entry_type)) {
                return;
        }

        smpc_smc_sshoff_call();

        void (*slave_entry)(void);
        slave_entry = _slave_polling_entry;

        if (entry_type == CPU_DUAL_ENTRY_ICI) {
                slave_entry = _slave_ici_entry;

                uint32_t interrupt;
                interrupt = INTC_INTERRUPT_SLAVE_BASE + INTC_INTERRUPT_FRT_ICI;

                cpu_intc_ihr_set(interrupt, _slave_ici_handler);
        }

        _slave_entry_type = -1;
        _slave_entry = _default_entry;

        if (entry != NULL) {
                _slave_entry_type = entry_type;
                _slave_entry = entry;
        }

        cpu_intc_ihr_set(INTC_INTERRUPT_SLAVE_ENTRY, slave_entry);

        smpc_smc_sshon_call();
}

static void __noreturn __aligned(16)
_slave_polling_entry(void)
{
        MEMORY_WRITE_AND(8, CPU(TIER), ~0x80);

        cpu_cache_purge();

        while (true) {
                cpu_dual_notification_wait();

                _slave_entry();
        }
}

static void __noreturn
_slave_ici_entry(void)
{
        cpu_intc_mask_set(15);

        MEMORY_WRITE_AND(8, CPU(TIER), ~0x8E);
        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x8F);

        MEMORY_WRITE_OR(8, CPU(TIER), 0x80);

        cpu_intc_mask_set(0);

        cpu_cache_purge();

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
