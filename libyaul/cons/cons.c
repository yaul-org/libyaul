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

#define CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES 0
#define CONS_ATTRIBUTE_BRIGHT   1
#define CONS_ATTRIBUTE_DIM      2
#define CONS_ATTRIBUTE_UNDERSCORE 4
#define CONS_ATTRIBUTE_BLINK    5 
#define CONS_ATTRIBUTE_REVERSE  7
#define CONS_ATTRIBUTE_HIDDEN   8

#define CONS_PALETTE_FG_BLACK   30
#define CONS_PALETTE_FG_RED     31
#define CONS_PALETTE_FG_GREEN   32
#define CONS_PALETTE_FG_YELLOW  33
#define CONS_PALETTE_FG_BLUE    34
#define CONS_PALETTE_FG_MAGENTA 35
#define CONS_PALETTE_FG_CYAN    36
#define CONS_PALETTE_FG_WHITE   37
#define CONS_PALETTE_FG_NORMALIZE(c) ((c) - CONS_PALETTE_FG_BLACK)

#define CONS_PALETTE_BG_BLACK   40
#define CONS_PALETTE_BG_RED     41
#define CONS_PALETTE_BG_GREEN   42
#define CONS_PALETTE_BG_YELLOW  43
#define CONS_PALETTE_BG_BLUE    44
#define CONS_PALETTE_BG_MAGENTA 45
#define CONS_PALETTE_BG_CYAN    46
#define CONS_PALETTE_BG_WHITE   47
#define CONS_PALETTE_BG_NORMALIZE(c) ((c) - CONS_PALETTE_BG_BLACK)

static void print_character(struct cons *, int, uint8_t, uint8_t);
static void print_escape_character(struct cons *, int);

static bool cursor_column_exceeded(struct cons *, int16_t);
static bool cursor_row_exceeded(struct cons *, uint32_t);
static void cursor_column_advance(struct cons *, int16_t);
static void cursor_column_reset(struct cons *cons);
static void cursor_row_advance(struct cons *, uint16_t);

static void vt_parser_callback(vt_parse_t *, vt_parse_action_t, int);

void
cons_write(struct cons *cons, const char *s)
{
        size_t slen;

        if ((slen = strlen(s)) == 0)
                return;

        vt_parse(&cons->vt_parser, s, slen);
}

void
cons_reset(struct cons *cons)
{

        vt_parse_init(&cons->vt_parser, vt_parser_callback, cons);

        cons->reset(cons);

        cons->cursor.col = 0;
        cons->cursor.row = 0;
}

static void
vt_parser_callback(vt_parse_t *parser, vt_parse_action_t action, int ch)
{
        static uint8_t fg = CONS_PALETTE_FG_WHITE;
        static uint8_t bg = CONS_PALETTE_BG_BLACK;

        struct cons *cons;

        uint8_t attr;
        uint8_t tmp;

        cons = (struct cons *)parser->user_data;
        switch (action) {
        case VT_PARSE_ACTION_PRINT:
                print_character(cons, ch, fg, bg);
        case VT_PARSE_ACTION_EXECUTE:
                print_escape_character(cons, ch);
                break;
        case VT_PARSE_ACTION_CSI_DISPATCH:
                switch (ch) {
                case 'm':
                        /* Set Attribute Mode: '<ESC>[{attr1};...;{attrn}m' */
                        if (parser->num_params == 0) {
                                fg = CONS_PALETTE_FG_WHITE;
                                bg = CONS_PALETTE_BG_BLACK;
                                break;
                        }

                        if (parser->num_params > 2)
                                break;

                        /* Attribute */
                        switch (parser->params[0]) {
                        case CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES:
                                fg = CONS_PALETTE_FG_WHITE;
                                bg = CONS_PALETTE_BG_BLACK;
                                return;
                        case CONS_ATTRIBUTE_BRIGHT:
                                attr = 8;
                                break;
                        case CONS_ATTRIBUTE_DIM:
                                attr = 0;
                                return;
                        case CONS_ATTRIBUTE_UNDERSCORE:
                                attr = 0;
                                return;
                        case CONS_ATTRIBUTE_BLINK:
                                attr = 0;
                                return;
                        case CONS_ATTRIBUTE_REVERSE:
                                tmp = fg;
                                fg = bg;
                                bg = tmp;
                                return;
                        case CONS_ATTRIBUTE_HIDDEN:
                                return;
                        default:
                                attr = 0;
                                break;
                        }

                        /* Foreground & background */
                        switch (parser->params[1]) {
                        case CONS_PALETTE_FG_BLACK:
                        case CONS_PALETTE_FG_RED:
                        case CONS_PALETTE_FG_GREEN:
                        case CONS_PALETTE_FG_YELLOW:
                        case CONS_PALETTE_FG_BLUE:
                        case CONS_PALETTE_FG_MAGENTA:
                        case CONS_PALETTE_FG_CYAN:
                        case CONS_PALETTE_FG_WHITE:
                                fg = parser->params[1] + attr;
                                break;
                        case CONS_PALETTE_BG_BLACK:
                        case CONS_PALETTE_BG_RED:
                        case CONS_PALETTE_BG_GREEN:
                        case CONS_PALETTE_BG_YELLOW:
                        case CONS_PALETTE_BG_BLUE:
                        case CONS_PALETTE_BG_MAGENTA:
                        case CONS_PALETTE_BG_CYAN:
                        case CONS_PALETTE_BG_WHITE:
                                bg = parser->params[1] + attr;
                                break;
                        }
                        break;
                }
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
cursor_column_exceeded(struct cons *cons, int16_t x)
{
        int16_t col;

        col = cons->cursor.col + x;
        return (col < 0) || (col >= COLS);
}

/*
 * Return true if the current cursor position plus an Y amount of rows
 * is out of bounds.
 */
static bool __attribute__ ((unused))
cursor_row_exceeded(struct cons *cons, uint32_t x)
{

        return (cons->cursor.col + x) >= COLS;
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
 * Set the cursor back to the leftmost column.
 */
static void __attribute__ ((unused))
cursor_column_reset(struct cons *cons)
{

        cons->cursor.col = 0;
}
/*
 * Advance the cursor an X amount of columns.
 */
static void __attribute__ ((unused))
cursor_column_advance(struct cons *cons, int16_t x)
{

        cons->cursor.col += x;
}

static void
print_character(struct cons *cons, int ch, uint8_t fg, uint8_t bg)
{

        if (cursor_column_exceeded(cons, 0)) {
                cursor_column_reset(cons);
                cursor_row_advance(cons, 1);
        }

        cons->write(cons, (char)ch,
            CONS_PALETTE_FG_NORMALIZE(fg),
            CONS_PALETTE_BG_NORMALIZE(bg));
        cursor_column_advance(cons, 1);
}

static void
print_escape_character(struct cons *cons, int ch)
{
        int16_t tab;

        switch (ch) {
        case '\0':
                break;
        case '\a':
                break;
        case '\b':
                if (!cursor_column_exceeded(cons, -1))
                        cursor_column_advance(cons, -1);
                break;
        case '\f':
                break;
        case '\n':
                cursor_column_reset(cons);
                cursor_row_advance(cons, 1);
                break;
        case '\r':
                cursor_column_reset(cons);
                break;
        case '\t':
                tab = TAB_WIDTH;

                if (cursor_column_exceeded(cons, TAB_WIDTH)) {
                        if ((tab = (COLS - cons->cursor.col - 1)) < 0)
                                break;
                }

                cursor_column_advance(cons, tab);
                break;
        case '\v':
                cursor_row_advance(cons, 1);
                break;
        }
}
