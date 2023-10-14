/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include <cpu/frt.h>
#include <cpu/intc.h>

#include <gamemath/defs.h>

#include "internal.h"

#if MIC3D_PERF == 1
static void _frt_ovi_handler(void);

static uint32_t _absolute_ticks_get(void);

void
__perf_init(void)
{
    cpu_frt_init(CPU_FRT_CLOCK_DIV_8);
    cpu_frt_ovi_set(_frt_ovi_handler);

    cpu_frt_interrupt_priority_set(15);

    cpu_frt_count_set(0);

    __state.perf->overflow_count = 0;
    __state.perf->active_counters = 0;
}

void
__perf_counter_init(perf_counter_t *perf_counter)
{
    perf_counter->ticks = 0;
    perf_counter->start_tick = 0;
    perf_counter->end_tick = 0;
    perf_counter->max_ticks = 0;
}

void
__perf_counter_start(perf_counter_t *perf_counter)
{
    if (__state.perf->active_counters == 0) {
        const uint32_t sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        cpu_frt_count_set(0);
        __state.perf->overflow_count = 0;

        cpu_intc_mask_set(sr_mask);
    }

    perf_counter->start_tick = _absolute_ticks_get();

    __state.perf->active_counters++;
}

void
__perf_counter_end(perf_counter_t *perf_counter)
{
    perf_counter->end_tick = _absolute_ticks_get();
    perf_counter->ticks = perf_counter->end_tick - perf_counter->start_tick;
    perf_counter->max_ticks = max(perf_counter->ticks, perf_counter->max_ticks);

    __state.perf->active_counters--;
}

size_t
__perf_str(uint32_t ticks, char *buffer)
{
    /* Check clock: 320 or 352 */
    const fix16_t time = fix16_div(ticks << 8, CPU_FRT_NTSC_320_8_COUNT_1MS << 8);

    return fix16_str(time, buffer, 1);
}

static uint32_t
_absolute_ticks_get(void)
{
    const uint32_t ticks_remaining = cpu_frt_count_get();
    const uint32_t overflow_ticks = __state.perf->overflow_count * 65536;

    const uint32_t ticks = (ticks_remaining + overflow_ticks);

    return ticks;
}

static void
_frt_ovi_handler(void)
{
    __state.perf->overflow_count++;
}
#endif /* MIC3D_PERF */
