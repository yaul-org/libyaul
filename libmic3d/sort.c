#include <math.h>

#include "internal.h"

static inline void __always_inline
_singles_reset(void)
{
        __state.sort->singles_top = __state.sort->singles_pool + 1;
        __state.sort->singles_index = 0;
}

static inline sort_single_t * __always_inline
_singles_alloc(void)
{
        sort_single_t * const single = __state.sort->singles_top;

        __state.sort->singles_top++;
        __state.sort->singles_index++;

        return single;
}

void
__sort_init(void)
{
        extern sort_list_t __pool_sort_lists[];
        extern sort_single_t __pool_sort_singles[];

        __state.sort->singles_pool = __pool_sort_singles;
        __state.sort->sort_lists_pool = __pool_sort_lists;

        __sort_start();
}

void
__sort_start(void)
{
        _singles_reset();

        __state.sort->max_depth = 0;
}

void
__sort_insert(vdp1_link_t cmdt_link, int32_t z)
{
        const uint32_t index = clamp(z, 0, SORT_DEPTH - 1);

        __state.sort->max_depth = max(index, __state.sort->max_depth);

        sort_list_t * const list_head = &__state.sort->sort_lists_pool[z];

        sort_single_t * const new_single = _singles_alloc();
        const uint32_t new_index = __state.sort->singles_index;

        new_single->link = cmdt_link;
        new_single->next_single = list_head->head;

        list_head->head = new_index;
}

void
__sort_iterate(sort_iterate_t iterate)
{
        sort_list_t *list_head;
        list_head = &__state.sort->sort_lists_pool[__state.sort->max_depth];

        for (uint32_t i = 0; i <= __state.sort->max_depth; i++, list_head--) {
                if (list_head->head == 0) {
                        continue;
                }

                const sort_single_t *single;
                single = &__state.sort->singles_pool[list_head->head];

                while (true) {
                        iterate(single);

                        if (single->next_single == 0) {
                                break;
                        }

                        single = &__state.sort->singles_pool[single->next_single];
                }

                list_head->head = 0;
        }
}
