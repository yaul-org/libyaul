/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <smpc/smc.h>

#include <cpu/instructions.h>
#include <cpu/frt.h>
#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/slave.h>

static void _slave_entry(void);
static void _default_entry(void);

static void (*_entry)(void) = _default_entry;

void
cpu_slave_init(void)
{
        smpc_smc_sshoff_call();

        cpu_slave_entry_clear();

        cpu_intc_ihr_set(INTC_INTERRUPT_SLAVE_ENTRY, _slave_entry);

        smpc_smc_sshon_call();
}

void
cpu_slave_entry_set(void (*entry)(void))
{
        _entry = (entry != NULL) ? entry : _default_entry;
}

static void
_slave_entry(void)
{
        while (true) {
                while (((cpu_frt_status_get()) & FRTCS_ICF) == 0x00);

                cpu_frt_control_chg((uint8_t)~FRTCS_ICF);

                _entry();
        }
}

static void
_default_entry(void)
{
}
