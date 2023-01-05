/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <stdio.h>

#include <bios-internal.h>

#include <cpu/intc.h>
#include <cpu/dual.h>
#include <cpu/map.h>
#include <cpu/frt.h>

static cpu_frt_ihr_t *_ihr_table_get(void);

static void _frt_oci_handler(cpu_frt_ihr_t *ihr_table);
static void _frt_ovi_handler(cpu_frt_ihr_t *ihr_table);

static void _master_oci_handler(void);
static void _slave_oci_handler(void);

static void _master_ovi_handler(void);
static void _slave_ovi_handler(void);

#define IHR_INDEX_NONE 0
#define IHR_INDEX_OCBI 1
#define IHR_INDEX_OCAI 2
#define IHR_INDEX_OVI  3

static cpu_frt_ihr_t _master_ihr_table[] = {
        __BIOS_DEFAULT_HANDLER,
        __BIOS_DEFAULT_HANDLER,
        __BIOS_DEFAULT_HANDLER,
        __BIOS_DEFAULT_HANDLER
};

static cpu_frt_ihr_t _slave_ihr_table[] = {
        __BIOS_DEFAULT_HANDLER,
        __BIOS_DEFAULT_HANDLER,
        __BIOS_DEFAULT_HANDLER,
        __BIOS_DEFAULT_HANDLER
};

static cpu_frt_ihr_t * const _ihr_tables[] = {
        _master_ihr_table,
        _slave_ihr_table
};

void
cpu_frt_init(uint8_t clock_div)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        cpu_map->tier &= ~0x8E;
        cpu_map->ftcsr &= ~0x8F;

        cpu_map->vcrc = (CPU_INTC_INTERRUPT_FRT_ICI << 8) | CPU_INTC_INTERRUPT_FRT_OCI;
        cpu_map->vcrd = CPU_INTC_INTERRUPT_FRT_OVI << 8;

        cpu_frt_interrupt_priority_set(15);

        /* Set internal clock (divisor) */
        cpu_map->tcr &= ~0x83;
        cpu_map->tcr |= clock_div & 0x03;

        cpu_frt_oca_clear();
        cpu_frt_ocb_clear();
        cpu_frt_ovi_clear();

        const cpu_which_t which_cpu = cpu_dual_executor_get();

        if (which_cpu == CPU_MASTER) {
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_FRT_OCI, _master_oci_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_FRT_OVI, _master_ovi_handler);

                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_FRT_OCI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _slave_oci_handler);
                cpu_intc_ihr_set(CPU_INTC_INTERRUPT_FRT_OVI + CPU_INTC_INTERRUPT_SLAVE_BASE,
                    _slave_ovi_handler);
        }

        cpu_frt_count_set(0);
}

void
cpu_frt_oca_set(uint16_t count, cpu_frt_ihr_t ihr)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        /* Disable interrupt */
        cpu_map->tier &= ~0x08;
        cpu_map->ftcsr &= ~0x08;

        /* Select OCRA register and select output compare A match */
        cpu_map->tocr &= ~0x12;
        cpu_map->ocrah = 0x00;
        cpu_map->ocral = 0x00;

        cpu_frt_ihr_t * const ihr_table = _ihr_table_get();

        ihr_table[IHR_INDEX_OCAI] = __BIOS_DEFAULT_HANDLER;

        if ((count > 0) && (ihr != NULL)) {
                cpu_map->tocr &= ~0x10;
                cpu_map->ocrah = (uint8_t)(count >> 8);
                cpu_map->ocral = (uint8_t)(count & 0xFF);

                /* Compare on match A */
                cpu_map->tocr |= 0x02;
                cpu_map->tier |= 0x08;

                ihr_table[IHR_INDEX_OCAI] = ihr;
        }
}

void
cpu_frt_ocb_set(uint16_t count, cpu_frt_ihr_t ihr)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        /* Disable interrupt */
        cpu_map->tier &= ~0x04;
        cpu_map->ftcsr &= ~0x04;

        /* Select output compare B match */
        cpu_map->tocr &= ~0x01;

        /* Select OCRB register */
        cpu_map->tocr |= 0x10;
        cpu_map->ocrbh = 0x00;
        cpu_map->ocrbl = 0x00;

        cpu_frt_ihr_t * const ihr_table = _ihr_table_get();

        ihr_table[IHR_INDEX_OCBI] = __BIOS_DEFAULT_HANDLER;

        if ((count > 0) && (ihr != NULL)) {
                cpu_map->ocrbh = (uint8_t)(count >> 8);
                cpu_map->ocrbl = (uint8_t)(count & 0xFF);

                cpu_map->tocr |= 0x01;
                cpu_map->tier |= 0x04;

                ihr_table[IHR_INDEX_OCBI] = ihr;
        }
}

void
cpu_frt_ovi_set(cpu_frt_ihr_t ihr)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        cpu_map->tier &= ~0x02;
        cpu_map->ftcsr &= ~0x02;

        cpu_frt_ihr_t * const ihr_table = _ihr_table_get();

        ihr_table[IHR_INDEX_OVI] = __BIOS_DEFAULT_HANDLER;

        if (ihr != NULL) {
                ihr_table[IHR_INDEX_OVI] = ihr;

                cpu_map->tier |= 0x02;
        }
}

static void __interrupt_handler
_master_oci_handler(void)
{
        _frt_oci_handler(_master_ihr_table);
}

static void __interrupt_handler
_slave_oci_handler(void)
{
        _frt_oci_handler(_slave_ihr_table);
}

static void
_frt_oci_handler(cpu_frt_ihr_t *ihr_table)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        const uint8_t ftcsr_bits = cpu_map->ftcsr;

        /* Disable OCA or OCB interrupt (or neither), invoke the callback and
         * enable interrupt again */

        const uint32_t ocf_bits = ftcsr_bits & 0x0C;

        cpu_map->tier &= ~ocf_bits;
        cpu_map->ftcsr = ftcsr_bits & ~0x0C;

        ihr_table[(ocf_bits & 0x08) >> 2]();
        ihr_table[(ocf_bits & 0x04) >> 2]();

        cpu_map->tier |= ocf_bits;
}

static void __interrupt_handler
_master_ovi_handler(void)
{
        _frt_ovi_handler(_master_ihr_table);
}

static void __interrupt_handler
_slave_ovi_handler(void)
{
        _frt_ovi_handler(_slave_ihr_table);
}

static void
_frt_ovi_handler(cpu_frt_ihr_t *ihr_table)
{
        volatile cpu_map_t * const cpu_map = (volatile cpu_map_t *)CPU_MAP_BASE;

        cpu_map->tier &= ~0x02;
        cpu_map->ftcsr &= ~0x02;

        ihr_table[IHR_INDEX_OVI]();

        cpu_map->tier |= 0x02;
}

static cpu_frt_ihr_t *
_ihr_table_get(void)
{
        const cpu_which_t which_cpu = cpu_dual_executor_get();

        return _ihr_tables[which_cpu];
}
