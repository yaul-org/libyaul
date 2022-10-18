/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _G3D_PERF_H_
#define _G3D_PERF_H_

#include <stdint.h>

typedef struct perf_counter {
        uint32_t start_tick;
        uint32_t end_tick;
        uint32_t ticks;
        uint32_t max_ticks;
} perf_counter_t;

void __perf_init(void);

void perf_counter_init(perf_counter_t *perf_counter);
void perf_counter_start(perf_counter_t *perf_counter);
void perf_counter_end(perf_counter_t *perf_counter);

#endif /* !_G3D_PERF_H_ */
