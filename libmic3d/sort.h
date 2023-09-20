/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_SORT_H_
#define _MIC3D_SORT_H_

#include <assert.h>
#include <stdint.h>

#include "render.h"

typedef struct sort_single {
    int16_t next_single;
} __aligned(2) sort_single_t;

typedef struct {
    uint16_t head;
} __aligned(2) sort_list_t;

typedef struct sort {
    sort_single_t *singles_pool;
    sort_list_t *sort_lists_pool;

    sort_single_t *singles_top;
    uint32_t singles_index;
    uint32_t max_depth;
} sort_t;

typedef void (*sort_iterate_t)(const sort_single_t *single);

void __sort_init(void);
void __sort_reset(void);
void __sort_insert(int32_t z);
void __sort_iterate(sort_iterate_t iterate);

#endif /* _MIC3D_SORT_H_ */
