/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * misscelan
 */

#include <assert.h>
#include <string.h>

#include "sega3d.h"

#include "sega3d-internal.h"

static struct {
        sort_single_t *current; 
} _state;

static const sort_single_t _single_empty = {
        .packet      = NULL,
        .next_single = NULL
};

void
_internal_sort_init(void)
{
        _state.current = &_internal_state->sort_single_pool[0];

        (void)memset(_internal_state->sort_list, 0, sizeof(sort_list_t) * SORT_Z_RANGE);
        (void)memset(_internal_state->sort_single_pool, 0, sizeof(sort_single_t) * PACKET_SIZE);
}

void
_internal_sort_clear(void)
{
        _internal_sort_init();
}

void
_internal_sort_add(void *packet, int32_t pz)
{
        if (pz > (SORT_Z_RANGE - 1)) {
                pz = SORT_Z_RANGE - 1;
        }

        if (pz < 0) {
                pz = 0;
        }

        /* Get the node */
        sort_single_t **free_link;
        free_link = &_internal_state->sort_list[pz].last_single;

        sort_single_t *next_available;
        next_available = _state.current; 

        /* Assign the value to this new node */
        next_available->packet = packet;

        /* There was a package here already in this Z */
        if (*free_link != NULL) {
                /* Update the following z link */
                next_available->next_single = *free_link;
        } else {
                /* Fill in the first z with the blank node */
                next_available->next_single = (sort_single_t *)&_single_empty;
        }

        *free_link = _state.current++;
}

void
_internal_sort_iterate(iterate_fn fn)
{
        assert(fn != NULL);

        const sort_single_t **first_single =
            (const sort_single_t **)&_internal_state->sort_list[SORT_Z_RANGE - 1].last_single;

        for (int32_t i = 0; i < SORT_Z_RANGE; i++, first_single--) {
                /* There isn't something in this Z */
                if (*first_single == NULL) {
                        continue;
                }

                /* Send commmand here */
                fn(*first_single);

                if ((*first_single)->next_single != NULL) {
                        const sort_single_t *single_next;
                        single_next = (*first_single)->next_single;

                        while (single_next->next_single != NULL) {
                                /* Send commmand here */
                                fn(*first_single);

                                single_next = single_next->next_single;
                        }
                }
        }
}
