#include <math.h>

#include <cpu/frt.h>

#include "perf.h"

static void _frt_ovi_handler(void);

static volatile struct {
        uint32_t overflow_count;
} _state;

static uint32_t _absolute_ticks_get(void);

void
__perf_init(void)
{
        cpu_frt_init(CPU_FRT_CLOCK_DIV_8);
        cpu_frt_ovi_set(_frt_ovi_handler);

        cpu_frt_interrupt_priority_set(15);

        cpu_frt_count_set(0);

        _state.overflow_count = 0;
}

void
perf_counter_init(perf_counter_t *perf_counter)
{
        perf_counter->ticks = 0;
        perf_counter->max_ticks = 0;
}

void
perf_counter_start(perf_counter_t *perf_counter)
{
        perf_counter->start_tick = _absolute_ticks_get();
}

void
perf_counter_end(perf_counter_t *perf_counter)
{
        perf_counter->end_tick = _absolute_ticks_get();
        perf_counter->ticks = perf_counter->end_tick - perf_counter->start_tick;

        perf_counter->max_ticks = max(perf_counter->ticks, perf_counter->max_ticks);
}

static uint32_t
_absolute_ticks_get(void)
{
        const uint32_t ticks_remaining = cpu_frt_count_get();
        const uint32_t overflow_ticks = _state.overflow_count * 65536;

        const uint32_t ticks = (ticks_remaining + overflow_ticks);

        return ticks;
}

static void
_frt_ovi_handler(void)
{
        _state.overflow_count++;
}
