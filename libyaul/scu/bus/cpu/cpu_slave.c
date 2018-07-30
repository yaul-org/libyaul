/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <smpc/smc.h>

#include <cpu/cache.h>
#include <cpu/frt.h>
#include <cpu/instructions.h>
#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/slave.h>

static void _slave_entry(void);

static void _ici_handler(void);

static void _default_entry(void);
static void _default_ihr(void);

static void (*_entry)(void) = _default_entry;

static void (*_ici_ihr)(void) = _default_ihr;

void
cpu_slave_init(void)
{
        smpc_smc_sshoff_call();

        cpu_slave_entry_clear(CPU_MASTER);
        cpu_slave_entry_clear(CPU_SLAVE);

        cpu_intc_ihr_set(INTC_INTERRUPT_SLAVE_ENTRY, _slave_entry);

        cpu_intc_ihr_set(INTC_INTERRUPT_FRT_ICI, _ici_handler);

        smpc_smc_sshon_call();
}

void
cpu_slave_entry_set(uint8_t cpu, void (*entry)(void))
{
        switch (cpu & 0x01) {
        case CPU_MASTER: {
                volatile uint8_t *reg_tier;
                reg_tier = (volatile uint8_t *)CPU(TIER);

                *reg_tier &= ~0x80;

                MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

                _ici_ihr = _default_ihr;

                if (entry != NULL) {
                        _ici_ihr = entry;

                        *reg_tier |= 0x80;
                }
        } break;
        case CPU_SLAVE:
                _entry = (entry != NULL) ? entry : _default_entry;
                break;
        }
}

static void __noreturn __aligned(16)
_slave_entry(void)
{
        MEMORY_WRITE_AND(8, CPU(TIER), ~0x80);

        while (true) {
                cpu_slave_notification_wait();

                _entry();
        }
}

static void __attribute__ ((interrupt_handler))
_ici_handler(void)
{
        volatile uint8_t *reg_tier;
        reg_tier = (volatile uint8_t *)CPU(TIER);

        *reg_tier &= ~0x80;

        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

        _ici_ihr();

        *reg_tier |= 0x80;
}

static void
_default_entry(void)
{
}

static void
_default_ihr(void)
{
}
