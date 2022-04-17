/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * misscelan
 */

#include <assert.h>
#include <string.h>
#include <math.h>

#include "g3d.h"

#include "g3d-internal.h"

static struct {
        /* Top pointer to the pool of singles */
        sort_single_t *pool_free_top;
        /* The maximum indexed Z value when adding packets */
        int32_t max_z;
} _state;

static inline void __always_inline
_pool_stack_reset(void)
{
        _state.pool_free_top = &__state->sort_single_pool[0];
}

static inline sort_single_t * __always_inline
_pool_stack_alloc(void)
{
        sort_single_t * const single = _state.pool_free_top;

        _state.pool_free_top++;

        return single;
}

static inline void __always_inline
_state_reset(void)
{
        _pool_stack_reset();

        _state.max_z = 0;
}

void
__sort_init(void)
{
        _state_reset();

        (void)memset(__state->sort_list, 0, sizeof(sort_list_t) * SORT_Z_RANGE);
}

void
__sort_add(void *packet, int32_t pz)
{
        pz = clamp(pz, 0, SORT_Z_RANGE - 1);

        _state.max_z = max(pz, _state.max_z);

        sort_list_t * const list_head = &__state->sort_list[pz];

        sort_single_t * const new_single = _pool_stack_alloc();

        new_single->packet = packet;
        new_single->next_single = list_head->head;

        list_head->head = new_single;
}

void
__sort_iterate(sort_iterate_fn_t iterate_fn)
{
        assert(iterate_fn != NULL);

        sort_list_t *list_head;
        list_head = &__state->sort_list[SORT_Z_RANGE - 1];

        for (int32_t i = 0; i < SORT_Z_RANGE; i++, list_head--) {
                const sort_single_t *single;
                single = list_head->head;

                while (single != NULL) {
                        iterate_fn(single);

                        single = single->next_single;
                }

                list_head->head = NULL;
        }

        _state_reset();
}
