/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <math.h>

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
        extern sort_list_t __pool_sort_lists[];
        extern sort_single_t __pool_sort_singles[];

        sort_t * const sort = __state.sort;

        sort->singles_pool = __pool_sort_singles;
        sort->sort_lists_pool = __pool_sort_lists;

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
__sort_insert(int32_t z)
{
        sort_t * const sort = __state.sort;

        const uint32_t index = clamp(z, 0, SORT_DEPTH - 1);

        sort->max_depth = max(index, sort->max_depth);

        sort_list_t * const list_head = &sort->sort_lists_pool[z];

        sort_single_t * const new_single = _singles_alloc();
        const uint32_t new_index = sort->singles_index;

        new_single->next_single = list_head->head;

        list_head->head = new_index;
}

void
__sort_iterate(sort_iterate_t iterate)
{
        sort_t * const sort = __state.sort;

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
                        iterate(single);

                        if (single->next_single == 0) {
                                break;
                        }

                        single = &sort->singles_pool[single->next_single];
                }
        }
}
