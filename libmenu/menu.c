/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <assert.h>
#include <stdlib.h>

#include <yaul.h>

#include "menu.h"

static inline void __always_inline
_menu_cursor_clamp(menu_t *menu, int8_t direction)
{
    int8_t cursor = menu->_cursor + direction;

    if (cursor < 0) {
        cursor = 0;
    } else if (cursor >= menu->_entries_count) {
        cursor = menu->_entries_count - 1;
    } else {
        const menu_entry_t * const menu_entry = &menu->entries[cursor];

        if (*menu_entry->label == '\0') {
            cursor = menu->_cursor;
        }
    }

    menu->_cursor = cursor;
}

void
menu_init(menu_t *menu)
{
    menu->entries = NULL;
    menu->flags = MENU_NONE;
    menu->data = NULL;

    menu->_cursor = 0;
    menu->_entries_count = 0;
    menu->_input_fn = NULL;
}

void
menu_input_set(menu_t *menu, menu_input_fn_t input_fn)
{
    menu->_input_fn = input_fn;
}

void
menu_entries_set(menu_t *menu, menu_entry_t *entries, uint32_t count)
{
    menu->entries = entries;

    if (menu->entries == NULL) {
        return;
    }

    menu->_entries_count = count;
}

void
menu_update(menu_t *menu)
{
    if ((menu->flags & MENU_ENABLED) != MENU_ENABLED) {
        return;
    }

    menu->current_entry = &menu->entries[menu->_cursor];

    for (menu_cursor_t cursor = 0; cursor < menu->_entries_count; cursor++) {
        menu_entry_t * const menu_entry = &menu->entries[cursor];

        const char cursor_buffer[2] = {
            (menu->_cursor == cursor) ? '' : ' ',
            '\0'
        };

        dbgio_puts(cursor_buffer);

        if (menu_entry->update_fn != NULL) {
            menu_entry->update_fn(menu, menu_entry);
        }

        dbgio_puts(menu_entry->label);
        dbgio_puts("\n");
    }

    if (menu->_input_fn != NULL) {
        menu->_input_fn(menu);
    }
}

menu_cursor_t
menu_cursor(menu_t *menu)
{
    return menu->_cursor;
}

void
menu_cursor_up_move(menu_t *menu)
{
    if ((menu->flags & MENU_ENABLED) != MENU_ENABLED) {
        return;
    }

    _menu_cursor_clamp(menu, -1);
}

void
menu_cursor_down_move(menu_t *menu)
{
    if ((menu->flags & MENU_ENABLED) != MENU_ENABLED) {
        return;
    }

    _menu_cursor_clamp(menu, 1);
}

void
menu_cycle_call(menu_t *menu, int32_t direction)
{
    if ((menu->flags & MENU_ENABLED) != MENU_ENABLED) {
        return;
    }

    menu_entry_t * const menu_entry = &menu->entries[menu->_cursor];
    menu_cycle_t cycle = menu_entry->cycle_fn;

    if (cycle != NULL) {
        cycle(menu, menu_entry, (direction < 0) ? -1 : 1);
    }
}

void
menu_action_call(menu_t *menu)
{
    if ((menu->flags & MENU_ENABLED) != MENU_ENABLED) {
        return;
    }

    menu_entry_t * const menu_entry = &menu->entries[menu->_cursor];
    menu_action_t action = menu_entry->action_fn;

    if (action != NULL) {
        action(menu, menu_entry);
    }
}
