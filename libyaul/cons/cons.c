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

#include "cons.h"

static bool cursor_column_exceeded(struct cons *, uint32_t);
static bool cursor_rows_exceeded(struct cons *, uint32_t);
static void cursor_column_advance(struct cons *, uint16_t);
static void cursor_newline_make(struct cons *);
static void cursor_row_advance(struct cons *, uint16_t);
static void terminal_reset(struct cons *);

static void vt_parser_callback(vt_parse_t *, vt_parse_action_t, uint8_t);

void
cons_write(struct cons *cons, const uint8_t *s)
{
        size_t slen;

        if ((slen = strlen((char *)s)) == 0)
                return;

        vt_parse(&cons->vt_parser, s, slen);
}

void
cons_reset(struct cons *cons)
{

        vt_parse_init(&cons->vt_parser, vt_parser_callback, cons);

        cons->cursor.col = 0;
        cons->cursor.row = 0;
}

static void
vt_parser_callback(vt_parse_t *parser, vt_parse_action_t action, uint8_t ch)
{
        struct cons *cons;

        cons = (struct cons *)parser->user_data;
        switch (action) {
        case VT_PARSE_ACTION_PRINT:
                /* XXX Provisional code */
                if ((cursor_column_exceeded(cons, 0)))
                        cursor_newline_make(cons);

                cons->write(cons, ch, 15, 0);
                cursor_column_advance(cons, 1);
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
cursor_column_exceeded(struct cons *cons, uint32_t x)
{

        return (cons->cursor.col + x) >= COLS;
}

/*
 * Return true if the current cursor position plus an Y amount of rows
 * is out of bounds.
 */
static bool __attribute__ ((unused))
cursor_rows_exceeded(struct cons *cons, uint32_t x)
{

        return (cons->cursor.col + x) >= COLS;
}

/*
 * Move the cursor to a new row and reset the cursor back to the
 * leftmost column.
 */
static void __attribute__ ((unused))
cursor_newline_make(struct cons *cons)
{

        cons->cursor.col = 0;
        cons->cursor.row++;
}

/*
 * Reset the terminal.
 */
static void __attribute__ ((unused))
terminal_reset(struct cons *cons)
{

        cons->cursor.col = 0;
        cons->cursor.row = 0;
}

/*
 * Advance the cursor an X amount of rows.
 */
static void __attribute__ ((unused))
cursor_row_advance(struct cons *cons, uint16_t x)
{

        cons->cursor.row += x;
}

/*
 * Advance the cursor an X amount of columns.
 */
static void __attribute__ ((unused))
cursor_column_advance(struct cons *cons, uint16_t x)
{

        cons->cursor.col += x;
}
