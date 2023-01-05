/*
 * Copyright (c) 2012-2021 Israel Jacquez
 * See LICENSE for details.
 *
 * Nikita Sokolov <hitomi2500@mail.ru>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>

#include <cpu/instructions.h>
#include <cpu/intc.h>
#include <cpu/sci.h>

#include <scu/ic.h>

#include <sys/callback-list.h>

#include "cpu-internal.h"

static void _eri_ihr_handler(void);
static void _rxi_ihr_handler(void);
static void _txi_ihr_handler(void);
static void _tei_ihr_handler(void);

#define IHR_INDEX_ERI 0
#define IHR_INDEX_RXI 1
#define IHR_INDEX_TXI 2
#define IHR_INDEX_TEI 3

static callback_t _master_ihr_callbacks[4];
static callback_t _slave_ihr_callbacks[4];

static callback_t * const _ihr_callbacks[] = {
        _master_ihr_callbacks,
        _slave_ihr_callbacks
};

static callback_t *_ihr_callbacks_get(void);

void
__cpu_sci_init(void)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        cpu_map->vcra = (CPU_INTC_INTERRUPT_SCI_ERI << 8) | CPU_INTC_INTERRUPT_SCI_RXI;
        cpu_map->vcrb = (CPU_INTC_INTERRUPT_SCI_TXI << 8) | CPU_INTC_INTERRUPT_SCI_TEI;

        cpu_sci_interrupt_priority_set(0);

        const cpu_which_t which_cpu = cpu_dual_executor_get();

        if (which_cpu == CPU_MASTER) {
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_ERI, _eri_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_RXI, _rxi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TXI, _txi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TEI, _tei_ihr_handler);

                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_ERI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _eri_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_RXI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _rxi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TXI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _txi_ihr_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_SCI_TEI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _tei_ihr_handler);
        }

        for (uint32_t i = 0; i < 4; i++) {
                callback_init(&_master_ihr_callbacks[i]);
                callback_init(&_slave_ihr_callbacks[i]);
        }
}

void
cpu_sci_config_set(const cpu_sci_cfg_t *cfg)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        const uint32_t reg_smr = ((cfg->mode & 0x01) << 7) |
                                 ((cfg->length & 0x01) << 6) |
                                 ((cfg->parity & 0x01) << 5) |
                                 ((cfg->parity_mode & 0x01) << 4) |
                                 ((cfg->stop_bit & 0x01) << 3) |
                                 ((cfg->mp & 0x01) << 2) |
                                 ((cfg->clock_div & 0x03) << 0);

        callback_t * const ihr_callbacks = _ihr_callbacks_get();
        callback_t * ihr_callback;

        if (cfg->ihr_eri != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[IHR_INDEX_ERI];
                callback_set(ihr_callback, cfg->ihr_eri, NULL);
        }

        if (cfg->ihr_rxi != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[IHR_INDEX_RXI];
                callback_set(ihr_callback, cfg->ihr_rxi, NULL);
        }

        if (cfg->ihr_txi != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[IHR_INDEX_TXI];
                callback_set(ihr_callback, cfg->ihr_txi, NULL);
        }

        if (cfg->ihr_tei != NULL) {
                /* Set interrupt handling routine */
                ihr_callback = &ihr_callbacks[IHR_INDEX_TEI];
                callback_set(ihr_callback, cfg->ihr_tei, NULL);
        }

        cpu_sci_disable();

        cpu_map->smr = reg_smr;
        cpu_map->brr = cfg->baudrate;
}

void
cpu_sci_with_dmac_enable(const cpu_sci_cfg_t *cfg)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        bool ihrs_used;
        ihrs_used = false;

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

        if (((cfg->mp & 0x01) == 0x01) && ihrs_used) {
                /* If interrupts are used in multiprocessor mode, enable the
                 * corresponding bit */
                reg_scr |= 0x08;
        }

        reg_scr |= 0x30; /* Enable RX & TX */

        cpu_map->scr = reg_scr & 0xCF;
        /* TODO: Correctly wait for 1 SCI clock period here */
        cpu_instr_nop();
        cpu_instr_nop();
        cpu_instr_nop();
        cpu_map->scr = reg_scr;
}

static void __interrupt_handler
_eri_ihr_handler(void)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        callback_t * const ihr_callbacks = _ihr_callbacks_get();
        callback_t * const ihr_callback = &ihr_callbacks[IHR_INDEX_ERI];

        callback_call(ihr_callback);

        cpu_map->ssr &= ~0x38;
}

static void __interrupt_handler
_rxi_ihr_handler(void)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        callback_t * const ihr_callbacks = _ihr_callbacks_get();
        callback_t * const ihr_callback = &ihr_callbacks[IHR_INDEX_RXI];

        callback_call(ihr_callback);

        cpu_map->ssr &= ~0x40;
}

static void __interrupt_handler
_txi_ihr_handler(void)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        callback_t * const ihr_callbacks = _ihr_callbacks_get();
        callback_t * const ihr_callback = &ihr_callbacks[IHR_INDEX_TXI];

        callback_call(ihr_callback);

        cpu_map->ssr &= ~0x80;
}

static void __interrupt_handler
_tei_ihr_handler(void)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        callback_t * const ihr_callbacks = _ihr_callbacks_get();
        callback_t * const ihr_callback = &ihr_callbacks[IHR_INDEX_TEI];

        callback_call(ihr_callback);

        cpu_map->ssr &= ~0x04;
}

static callback_t *
_ihr_callbacks_get(void)
{
        const uint8_t which_cpu = cpu_dual_executor_get();

        return _ihr_callbacks[which_cpu];
}
