/*
 * Copyright (c) 2012-2021 Israel Jacquez
 * See LICENSE for details.
 *
 * Nikita Sokolov <hitomi2500@mail.ru>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>
#include <cpu/sci.h>
#include <cpu/intc.h>
#include <scu/ic.h>

#include <sys/callback-list.h>

#include "cpu-internal.h"

static void _sci_eri_ihr_handler(void);
static void _sci_rxi_ihr_handler(void);
static void _sci_txi_ihr_handler(void);
static void _sci_tei_ihr_handler(void);

#define CPU_DMAC_IHR_INDEX_ERI 0
#define CPU_DMAC_IHR_INDEX_RXI 1
#define CPU_DMAC_IHR_INDEX_TXI 2
#define CPU_DMAC_IHR_INDEX_TEI 3

static callback_t _master_ihr_callbacks[4];
static callback_t _slave_ihr_callbacks[4];

static callback_t *_sci_executor_ihr_callbacks_get(void);

void
_internal_cpu_sci_init(void)
{
        MEMORY_WRITE(16, CPU(VCRA), (CPU_INTC_INTERRUPT_SCI_ERI << 8) | CPU_INTC_INTERRUPT_SCI_RXI);
        MEMORY_WRITE(16, CPU(VCRB), (CPU_INTC_INTERRUPT_SCI_TXI << 8) | CPU_INTC_INTERRUPT_SCI_TEI);

        cpu_sci_interrupt_priority_set(0);

        const uint8_t which_cpu = cpu_dual_executor_get();

        if (which_cpu == CPU_MASTER) {
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_ERI, _sci_eri_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_RXI, _sci_rxi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TXI, _sci_txi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TEI, _sci_tei_ihr_handler);

                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_ERI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _sci_eri_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_RXI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _sci_rxi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TXI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _sci_txi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TEI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _sci_tei_ihr_handler);
        }

        for (int i=0; i<4; i++) {
                callback_init(&_master_ihr_callbacks[i]);
                callback_init(&_slave_ihr_callbacks[i]);
        }
 }

void
cpu_sci_config_set(const cpu_sci_cfg_t *cfg)
{
        uint32_t reg_smr;
        reg_smr = ((cfg->mode & 0x01) << 7) |
                  ((cfg->length & 0x01) << 6) |
                  ((cfg->parity & 0x01) << 5) |
                  ((cfg->parity_mode & 0x01) << 4) |
                  ((cfg->stop_bit & 0x01) << 3) |
                  ((cfg->mp & 0x01) << 2) |
                  ((cfg->clock_div & 0x03) << 0);

        callback_t * const ihr_callbacks = _sci_executor_ihr_callbacks_get();
        callback_t * ihr_callback;

        if (cfg->ihr_eri != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_ERI];
                callback_set(ihr_callback, cfg->ihr_eri, NULL);
        }

        if (cfg->ihr_rxi != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_RXI];
                callback_set(ihr_callback, cfg->ihr_rxi, NULL);
        }

        if (cfg->ihr_txi != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_TXI];
                callback_set(ihr_callback, cfg->ihr_txi, NULL);
        }

        if (cfg->ihr_tei != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_TEI];
                callback_set(ihr_callback, cfg->ihr_tei, NULL);
        }

        cpu_sci_disable();
        MEMORY_WRITE(8, CPU(SMR), reg_smr);
        MEMORY_WRITE(8, CPU(BRR), cfg->baudrate);
}

void
cpu_sci_enable_with_dmac(const cpu_sci_cfg_t *cfg)
{
        bool ihrs_used = false;
                
        uint32_t reg_scr;
        reg_scr = ((cfg->sck_config & 0x03) << 0);

        if (cfg->ihr_rxi != NULL) {
                reg_scr |= 0x40;
                ihrs_used = true;
        }

        if (cfg->ihr_txi != NULL) {
                reg_scr |= 0x80;
                ihrs_used = true;
        }

        if ((cfg->mp & 0x01) && (true == ihrs_used)) { 
                /* If interrupts are used in multiprocessor mode, enable the corresponding bit */
                reg_scr |= 0x08;
        }

        reg_scr |= 0x30; // enable RX & TX

        MEMORY_WRITE(8, CPU(SCR), reg_scr & 0xCF);
        //TODO: correctly wait for 1 SCI clock period here
        __asm("mov r0,r0");
        __asm("mov r0,r0");
        __asm("mov r0,r0");
        MEMORY_WRITE(8, CPU(SCR), reg_scr);
}

static callback_t *
_sci_executor_ihr_callbacks_get(void)
{
        const uint8_t which_cpu = cpu_dual_executor_get();

        switch (which_cpu) {
        case CPU_MASTER:
                return &_master_ihr_callbacks[0];
        case CPU_SLAVE:
                return &_slave_ihr_callbacks[0];
        }

        return NULL;
}

static void __interrupt_handler
_sci_eri_ihr_handler(void)
{
        callback_t *ihr_callbacks;
        ihr_callbacks = _sci_executor_ihr_callbacks_get();
        callback_t *ihr_callback;
        ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_ERI];

        callback_call(ihr_callback);

        MEMORY_WRITE_AND(8, CPU(SSR), ~0x38);
}

static void __interrupt_handler
_sci_rxi_ihr_handler(void)
{
        callback_t *ihr_callbacks;
        ihr_callbacks = _sci_executor_ihr_callbacks_get();
        callback_t *ihr_callback;
        ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_RXI];

        callback_call(ihr_callback);

        MEMORY_WRITE_AND(8, CPU(SSR), ~0x40);
}

static void __interrupt_handler
_sci_txi_ihr_handler(void)
{
        callback_t *ihr_callbacks;
        ihr_callbacks = _sci_executor_ihr_callbacks_get();
        callback_t *ihr_callback;
        ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_TXI];

        callback_call(ihr_callback);

        MEMORY_WRITE_AND(8, CPU(SSR), ~0x80);
}

static void __interrupt_handler
_sci_tei_ihr_handler(void)
{
        callback_t *ihr_callbacks;
        ihr_callbacks = _sci_executor_ihr_callbacks_get();
        callback_t *ihr_callback;
        ihr_callback = &ihr_callbacks[CPU_DMAC_IHR_INDEX_TEI];

        callback_call(ihr_callback);

        MEMORY_WRITE_AND(8, CPU(SSR), ~0x04);
}
