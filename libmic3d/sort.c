/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <gamemath/defs.h>

#include "internal.h"

static inline void __always_inline
_singles_reset(void)
{
    sort_t * const sort = __state.sort;

    sort->singles_top = sort->singles_pool + 1;
    sort->singles_index = 0;
}

static inline sort_single_t * __always_inline
_singles_alloc(void)
{
    sort_t * const sort = __state.sort;

    sort_single_t * const single = sort->singles_top;

    sort->singles_top++;
    sort->singles_index++;

    return single;
}

void
__sort_init(void)
{
    workarea_mic3d_t * const workarea = __state.workarea;

    sort_t * const sort = __state.sort;

    sort->singles_pool = (void *)workarea->sort_singles;

    __sort_reset();
}

void
__sort_reset(void)
{
    sort_t * const sort = __state.sort;

    _singles_reset();

    sort->max_depth = 0;
}

void
__sort_insert(uint32_t z)
{
    sort_t * const sort = __state.sort;

    assert(sort->sort_lists_pool != NULL);

    /* Keep track of the max depth used to save time when iterating */
    sort->max_depth = max(z, sort->max_depth);

    sort_list_t * const list_head = &sort->sort_lists_pool[z];

    sort_single_t * const new_single = _singles_alloc();
    const uint32_t new_index = sort->singles_index;

    new_single->next_single = list_head->head;

    list_head->head = new_index;
}

void
__sort_iterate(void)
{
    sort_t * const sort = __state.sort;

    assert(sort->sort_lists_pool != NULL);

    sort_list_t *list_head;
    list_head = &sort->sort_lists_pool[sort->max_depth];

    for (; list_head >= sort->sort_lists_pool; list_head--) {
        if (list_head->head == 0) {
            continue;
        }

        const sort_single_t *single;
        single = &sort->singles_pool[list_head->head];

        list_head->head = 0;

        while (true) {
            __render_single(single);

            if (single->next_single == 0) {
                break;
            }

            single = &sort->singles_pool[single->next_single];
        }
    }
}
