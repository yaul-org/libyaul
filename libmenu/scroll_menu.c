/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include "scroll_menu.h"

static void _scroll(scroll_menu_t *menu, int8_t direction);

static void _menu_input(menu_t *menu);

void
scroll_menu_init(scroll_menu_t *scroll_menu)
{
        scroll_menu->view_height = 0;
        scroll_menu->top_index = 0;
        scroll_menu->bottom_index = 0;

        scroll_menu->entries = NULL;
        scroll_menu->flags = SCROLL_MENU_NONE;
        scroll_menu->data = NULL;

        scroll_menu->_cursor = 0;
        scroll_menu->_y = 0;
        scroll_menu->_gp = 0;

        menu_t *menu;
        menu = &scroll_menu->_menu;

        menu_init(menu);
        menu_input_set(menu, _menu_input);

        menu->data = scroll_menu;
}

void
scroll_menu_input_set(scroll_menu_t *menu, scroll_menu_fn_t input_fn)
{
        menu->_input_fn = input_fn;

        assert(menu->_input_fn != NULL);
}

void
scroll_menu_update_set(scroll_menu_t *menu, scroll_menu_fn_t update_fn)
{
        menu->_update_fn = update_fn;
}

void
scroll_menu_entries_set(scroll_menu_t *scroll_menu, menu_entry_t *entries,
    uint32_t count)
{
        scroll_menu->entries = entries;

        menu_t * const menu = &scroll_menu->_menu;

        menu_entries_set(menu, entries, count);
}

menu_cursor_t
scroll_menu_local_cursor(scroll_menu_t *menu)
{
        return menu->_y;
}

menu_cursor_t
scroll_menu_cursor(scroll_menu_t *menu)
{
        return menu->_cursor;
}

void
scroll_menu_cursor_up(scroll_menu_t *menu)
{
        menu_cursor_up_move(&menu->_menu);

        _scroll(menu, -1);
}

void
scroll_menu_cursor_down(scroll_menu_t *menu)
{
        menu_cursor_down_move(&menu->_menu);

        _scroll(menu, 1);
}

void
scroll_menu_action_call(scroll_menu_t *menu)
{
        if ((menu->flags & MENU_ENABLED) != MENU_ENABLED) {
                return;
        }

        menu_cursor_t cursor;
        cursor = menu->_gp;

        menu_entry_t *menu_entry = &menu->entries[cursor];
        menu_action_t action = menu_entry->action_fn;

        if (action != NULL) {
                action(menu, menu_entry);
        }
}

void
scroll_menu_update(scroll_menu_t *scroll_menu)
{
        if (scroll_menu->_update_fn != NULL) {
                scroll_menu->_update_fn(scroll_menu);
        }

        menu_t *menu;
        menu = &scroll_menu->_menu;

        /* Update flags */
        menu->flags &= ~MENU_MASK;
        menu->flags |= scroll_menu->flags & MENU_MASK;

        menu_update(menu);
}

static void
_scroll(scroll_menu_t *menu, int8_t direction)
{
        menu->_cursor += direction;

        if (menu->_cursor < 0) {
                menu->_cursor = 0;
        } else if (menu->_cursor > menu->bottom_index) {
                menu->_cursor = menu->bottom_index;
        }

        menu->_gp += direction;

        if (menu->_gp < 0) {
                menu->_gp = 0;

                menu->_y += direction;
        } else if (menu->_gp > menu->view_height) {
                menu->_gp = menu->view_height;

                menu->_y += direction;
        }

        if (menu->_y < menu->top_index) {
                menu->_y = menu->top_index;
        } else if ((menu->_y + menu->view_height) >= menu->bottom_index) {
                menu->_y = menu->bottom_index - menu->view_height - 1;
        }
}

static void
_menu_input(menu_t *menu)
{
        scroll_menu_t *scroll_menu;
        scroll_menu = menu->data;

        if (scroll_menu->_input_fn != NULL) {
                scroll_menu->_input_fn(scroll_menu);
        }
}
