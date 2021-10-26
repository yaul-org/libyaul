/*
 * Copyright (c) 2012-2019
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

#define SLAVE_ENTRY_TRAMPOLINE_EMIT(type)                                      \
__asm__ (".align 1\n"                                                          \
         "\n"                                                                  \
         ".local __slave_" __STRING(type) "_entry_trampoline\n"                \
         ".type __slave_" __STRING(type) "_entry_trampoline, @function\n"      \
         "\n"                                                                  \
         "__slave_" __STRING(type) "_entry_trampoline:\n"                      \
         "\tmov.l 1f, r15\n"                                                   \
         "\tmov.l 2f, r1\n"                                                    \
         "\tmov r15, r14\n"                                                    \
         "\tjmp @r1\n"                                                         \
         "\tnop\n"                                                             \
         ".align 4\n"                                                          \
         "1:\n"                                                                \
         ".long __slave_stack\n"                                               \
         "2:\n"                                                                \
         ".long __slave_" __STRING(type) "_entry\n")

typedef void (*slave_entry_t)(void);

static void _slave_init(void);

void _slave_polling_entry_trampoline(void);
void _slave_ici_entry_trampoline(void);

static void _slave_polling_entry(void);
static void _slave_ici_entry(void);

static void _master_ici_handler(void);
static void _slave_ici_handler(void);

static void _default_entry(void);

static cpu_dual_master_entry_t _master_entry = _default_entry;
static cpu_dual_slave_entry_t _slave_entry __section(".uncached") = _default_entry;

static slave_entry_t _slave_entry_table[] = {
        &_slave_polling_entry_trampoline,
        &_slave_ici_entry_trampoline
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
        volatile uint8_t * const reg_tier =
            (volatile uint8_t *)CPU(TIER);

        *reg_tier &= ~0x80;

        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

        _master_entry = _default_entry;

        if (entry != NULL) {
                _master_entry = entry;

                *reg_tier |= 0x80;
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

cpu_which_t
cpu_dual_executor_get(void)
{
        extern uint32_t _master_stack;
        extern uint32_t _master_stack_end;

        extern uint32_t _slave_stack;
        extern uint32_t _slave_stack_end;

        const uint32_t stack = cpu_reg_sp_get();

        if ((stack >= (uint32_t)&_master_stack_end) &&
            (stack <= (uint32_t)&_master_stack)) {
                return CPU_MASTER;
        }

        if ((stack >= (uint32_t)&_slave_stack_end) &&
            (stack <= (uint32_t)&_slave_stack)) {
                return CPU_SLAVE;
        }

        return -1;
}

static void
_slave_init(void)
{
        _internal_cpu_divu_init();
        cpu_frt_init(CPU_FRT_CLOCK_DIV_8);

        cpu_wdt_init(CPU_WDT_CLOCK_DIV_2);
        _internal_cpu_dmac_init();

        cpu_intc_mask_set(0);

        cpu_cache_purge();
}

SLAVE_ENTRY_TRAMPOLINE_EMIT(polling);

static void __noreturn __aligned(16) __used
_slave_polling_entry(void)
{
        _slave_init();

        while (true) {
                cpu_dual_notification_wait();

                _slave_entry();
        }
}

SLAVE_ENTRY_TRAMPOLINE_EMIT(ici);

static void __noreturn __used
_slave_ici_entry(void)
{
        _slave_init();

        MEMORY_WRITE_OR(8, CPU(TIER), 0x80);

        while (true) {
        }
}

static void __interrupt_handler
_master_ici_handler(void)
{
        volatile uint8_t *reg_tier;
        reg_tier = (volatile uint8_t *)CPU(TIER);

        *reg_tier &= ~0x80;

        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

        _master_entry();

        *reg_tier |= 0x80;
}

static void __interrupt_handler
_slave_ici_handler(void)
{
        volatile uint8_t * const reg_tier =
            (volatile uint8_t *)CPU(TIER);

        *reg_tier &= ~0x80;

        MEMORY_WRITE_AND(8, CPU(FTCSR), ~0x80);

        _slave_entry();

        *reg_tier |= 0x80;
}

static void
_default_entry(void)
{
}
