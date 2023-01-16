#ifndef MIC3D_SORT_H
#define MIC3D_SORT_H

#include <assert.h>
#include <stdint.h>

#include "render.h"

typedef struct sort_single {
        vdp1_link_t link;
        int16_t next_single;
} __aligned(4) sort_single_t;

static_assert(sizeof(sort_single_t) == 4);

typedef struct {
        int16_t head;
} __aligned(2) sort_list_t;

static_assert(sizeof(sort_list_t) == 2);

typedef struct sort {
        sort_single_t *singles_pool;
        sort_list_t *sort_lists_pool;

        sort_single_t *singles_top;
        uint32_t singles_index;
        uint32_t max_depth;
} sort_t;

typedef void (*sort_iterate_t)(const sort_single_t *single);

void __sort_init(void);
void __sort_start(void);
void __sort_insert(vdp1_link_t cmdt_link, int32_t z);
void __sort_iterate(sort_iterate_t iterate);

#endif /* MIC3D_SORT_H */
