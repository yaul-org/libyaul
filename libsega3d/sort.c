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
        uint32_t index;
        sort_list_t list[Z_RANGE];
        sort_single_t single_pool[PACKET_SIZE];
} _state;

void
_internal_sort_init(void)
{
        _state.index = 0;

        (void)memset(_state.list, 0, sizeof(sort_list_t) * Z_RANGE);
        /* XXX: Maybe we can remove this? Is there really a need to reset the
         *      pool? */
        (void)memset(_state.single_pool, 0, sizeof(sort_single_t) * PACKET_SIZE);
}

void
_internal_sort_clear(void)
{
        _internal_sort_init();
}

void
_internal_sort_add(void *packet, int32_t pz)
{
        if (pz > (Z_RANGE - 1)) {
                pz = Z_RANGE - 1;
        }

        if (pz < 0) {
                pz = 0;
        }

        /* Get the node */
        sort_single_t *free_link;
        free_link = _state.list[pz].first_single;

        /* There was a package here already in this Z */
        if (free_link != NULL) {
                /* Loop until we get a free node */
                while (free_link->next_single != NULL) {
                        free_link = (sort_single_t *)free_link->next_single;
                }

                /* We pick the new link that is going to hold our value */
                sort_single_t *next_avail_single =
                    &_state.single_pool[_state.index];

                /* Increase the packet counter */
                _state.index++;

                /* Assign the value to this new node */
                next_avail_single->packet = packet;

                /* Link the package with the available link */
                free_link->next_single = next_avail_single;
        } else {
                sort_single_t **first_single =
                    (sort_single_t **)&_state.list[pz].first_single;

                *first_single = (sort_single_t *)&_state.single_pool[_state.index];

                (*first_single)->packet = packet;
                (*first_single)->next_single = NULL;

                _state.index++;
        }
}

void
_internal_sort_iterate(iterate_fn fn)
{
        assert(fn != NULL);

        sort_single_t **first_single =
            &_state.list[Z_RANGE - 1].first_single;

        for (int32_t i = 0; i < Z_RANGE; i++, first_single--) {
                /* There is something in this Z */
                if (*first_single == NULL) {
                        continue;
                }

                /* Send commmand here */
                fn(*first_single);

                if ((*first_single)->next_single == NULL) {
                        continue;
                }

                sort_single_t *single_next =
                    (sort_single_t *)(*first_single)->next_single;

                if (single_next->next_single != NULL) {
                        while (single_next->next_single) {
                                /* Send commmand here */
                                (*fn)(single_next);

                                single_next = (sort_single_t *)single_next->next_single;
                        }
                }

                /* Send commmand here */
                fn(single_next);
        }
}
