/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_PERF_H_
#define _MIC3D_PERF_H_

#include <stdint.h>

typedef struct perf {
    volatile uint32_t overflow_count;
    volatile uint32_t active_counters;
} perf_t;

typedef struct perf_counter {
    uint32_t start_tick;
    uint32_t end_tick;
    uint32_t ticks;
    uint32_t max_ticks;
    uint32_t overflow_count;
} perf_counter_t;

#if MIC3D_PERF == 1
void __perf_init(void);

void __perf_counter_init(perf_counter_t *perf_counter);
void __perf_counter_start(perf_counter_t *perf_counter);
void __perf_counter_end(perf_counter_t *perf_counter);
size_t __perf_str(uint32_t ticks, char *buffer);
#else
#define __perf_init()

#define __perf_counter_init(x)
#define __perf_counter_start(x)
#define __perf_counter_end(x)
#define __perf_str(x, y)
#endif /* MIC3D_PERF */

#endif /* !_MIC3D_PERF_H_ */
