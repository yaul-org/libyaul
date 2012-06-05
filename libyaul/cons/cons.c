/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include <cons.h>

static bool cursor_columns_exceeded(terminal_t *, uint32_t);
static bool cursor_rows_exceeded(terminal_t *, uint32_t);
static void cursor_column_advance(terminal_t *, uint16_t);
static void cursor_newline_make(terminal_t *);
static void cursor_row_advance(terminal_t *, uint16_t);
static void terminal_reset(terminal_t *);

static void vt_parser_callback(vt_parse_t *, vt_parse_action_t, uint8_t);

void
cons_write(struct cons *cons, const char *s)
{
        size_t slen;

        if ((slen = strlen(s)) == 0)
                return;

        vt_parse(&cons->vt_parse, s, slen);
}

void
cons_reset(struct cons *cons)
{

        vt_parse_init(&cons->vt_parser, vt_parser_callback);

        cons->cursor.col = 0;
        cons->cursor.row = 0;
}

void
vt_parser_callback(vt_parse_t *parser, vt_parse_action_t action, uint8_t ch)
{
        switch (action) {
        case VT_PARSE_ACTION_PRINT:
                break;
        case VT_PARSE_ACTION_EXECUTE:
                break;
        case VT_PARSE_ACTION_CSI_DISPATCH:
                break;
        default:
                break;
        }
}

/*
 * Return true if the current cursor position plus an X amount of
 * columns is out of bounds.
 */
static bool __attribute__ ((unused))
cursor_columns_exceeded(terminal_t *t, uint32_t x)
{

        return (t->cursor.x + x) >= COLS;
}

/*
 * Return true if the current cursor position plus an Y amount of rows
 * is out of bounds.
 */
static bool __attribute__ ((unused))
cursor_rows_exceeded(terminal_t *t, uint32_t x)
{

        return (t->cursor.x + x) >= COLS;
}

/*
 * Move the cursor to a new row and reset the cursor back to the
 * leftmost column.
 */
static void __attribute__ ((unused))
cursor_newline_make(terminal_t *t)
{

        t->cursor.x = 0;
        t->cursor.y++;
}

/*
 * Reset the terminal.
 */
static void __attribute__ ((unused))
terminal_reset(terminal_t *t)
{

        t->cursor.x = 0;
        t->cursor.y = 0;
}

/*
 * Advance the cursor an X amount of rows.
 */
static void __attribute__ ((unused))
cursor_row_advance(terminal_t *t, uint16_t x)
{

        t->cursor.y += x;
}

/*
 * Advance the cursor an X amount of columns.
 */
static void __attribute__ ((unused))
cursor_column_advance(terminal_t *t, uint16_t x)
{

        t->cursor.x += x;
}
