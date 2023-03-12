/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "list.h"

#define LIST_ALLOC_ALIGN (16)

void
__list_alloc(list_t *list, uint16_t count)
{
    assert(count > 0);

    if (list->count < count) {
        if ((list->flags & LIST_FLAGS_ALLOCATED) == LIST_FLAGS_ALLOCATED) {
            free(list->buffer);
        }

        list->buffer = memalign(list->size * count, LIST_ALLOC_ALIGN);
        assert(list->buffer);

        list->count = count;

        list->flags |= LIST_FLAGS_ALLOCATED;
    }
}

void
__list_free(list_t *list)
{
    __list_set(list, NULL, 0);
}

void
__list_set(list_t *list, void *list_ptr, uint16_t count)
{
    if ((list->flags & LIST_FLAGS_ALLOCATED) == LIST_FLAGS_ALLOCATED) {
        free(list->buffer);

        list->flags &= ~LIST_FLAGS_ALLOCATED;
    }

    if ((list_ptr != NULL) && (count != 0)) {
        list->buffer = list_ptr;
        list->count = count;

        list->flags |= LIST_FLAGS_USER_ALLOCATED;
    } else {
        list->buffer = list->default_element;
        list->count = 0;

        list->flags &= ~LIST_FLAGS_USER_ALLOCATED;
    }
}
