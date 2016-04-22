/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include <common.h>

#include "cons.h"
#include "drivers.h"
#include "vt_parse/vt_parse.h"

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

static struct cons cons;
static vt_parse_t vt_parser;

static inline void action_character_print(int);
static inline void action_escape_character_print(int);
static inline void action_csi_dispatch_print(int, int *, int);

static inline void buffer_clear(int32_t, int32_t, int32_t, int32_t);
static inline void buffer_glyph_write(uint8_t);
static inline uint8_t buffer_attribute_read(void);
static inline void buffer_attribute_write(uint8_t);
static inline void buffer_fg_write(uint8_t);
static inline void buffer_bg_write(uint8_t);

static bool cursor_column_exceeded(int32_t);
static bool cursor_row_exceeded(uint32_t);
static void cursor_column_advance(int32_t);
static bool cursor_column_cond_set(int32_t);
static void cursor_column_set(int32_t);
static void cursor_row_advance(uint16_t);
static bool cursor_row_cond_set(int32_t);
static void cursor_row_set(int32_t);
static bool cursor_cond_set(int32_t, int32_t);

static void vt_parser_callback(vt_parse_t *, vt_parse_action_t, int);

void
cons_init(uint8_t driver, uint16_t cols, uint16_t rows)
{
        static struct cons_buffer _cons_buffer[CONS_COLS_MAX * CONS_ROWS_MAX];

        cons.buffer = &_cons_buffer[0];
        cons.cols = cols;
        cons.rows = rows;

        uint32_t col;
        uint32_t row;
        for (row = 0; row < cons.rows; row++) {
                for (col = 0; col < cons.cols; col++) {
                        struct cons_buffer *entry;
                        entry = &cons.buffer[col + (row * cons.cols)];

                        entry->attribute = CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES;
                        entry->fg = CONS_PALETTE_FG_WHITE;
                        entry->fg = CONS_PALETTE_BG_BLACK;
                        entry->glyph = '\0';
                }
        }

        vt_parse_init(&vt_parser, vt_parser_callback);

        cons.cursor.col = 0;
        cons.cursor.row = 0;

        switch (driver) {
        case CONS_DRIVER_VDP1:
                cons_vdp1_init(&cons);
                break;
        case CONS_DRIVER_VDP2:
                cons_vdp2_init(&cons);
                break;
        default:
                assert(false);
                return;
        }

        cons.initialized = true;
}

void
cons_buffer(const char *buffer)
{
        assert(cons.initialized);

        if (buffer == NULL) {
                return;
        }

        size_t len;

        if ((len = strlen(buffer)) == 0) {
                return;
        }

        vt_parse(&vt_parser, buffer, len);
}

void
cons_write(const char *buffer)
{
        assert(cons.initialized);

        if (buffer == NULL) {
                return;
        }

        size_t len;
        if ((len = strlen(buffer)) == 0) {
                return;
        }

        vt_parse(&vt_parser, buffer, len);

        cons.write(&cons);
}

void
cons_flush(void)
{
        assert(cons.initialized);

        cons.write(&cons);
}

static void
vt_parser_callback(vt_parse_t *parser, vt_parse_action_t action, int ch)
{
        switch (action) {
        case VT_PARSE_ACTION_PRINT:
                action_character_print(ch);
                break;
        case VT_PARSE_ACTION_EXECUTE:
                action_escape_character_print(ch);
                break;
        case VT_PARSE_ACTION_CSI_DISPATCH:
                action_csi_dispatch_print(ch, &parser->params[0], parser->num_params);
                break;
        default:
                break;
        }
}

/*
 * Return true if the current cursor position plus an X amount of
 * columns is out of bounds.
 */
static bool __unused
cursor_column_exceeded(int32_t x)
{
        int32_t col;

        col = cons.cursor.col + x;
        return (col < 0) || (col >= cons.cols);
}

/*
 * Return true if the current cursor position plus an Y amount of rows
 * is out of bounds.
 */
static bool __unused
cursor_row_exceeded(uint32_t y)
{
        int32_t row;

        row = cons.cursor.row + y;
        return (row < 0) || (row >= cons.rows);
}

/*
 * Advance the cursor an Y amount of rows.
 */
static void __unused
cursor_row_advance(uint16_t y)
{

        cons.cursor.row += y;
}

/*
 * Set the cursor an Y amount of rows.
 */
static void __unused
cursor_row_set(int32_t y)
{

        cons.cursor.row = y;
}

/*
 * Set the cursor to ROW and return TRUE iff the ROW has not been
 * exceeded.
 */
static bool __unused
cursor_row_cond_set(int32_t row)
{

        if ((row >= 0) && (row <= cons.rows)) {
                cons.cursor.row = row;
                return true;
        }

        return false;
}

/*
 * Advance the cursor an X amount of columns.
 */
static void __unused
cursor_column_advance(int32_t x)
{

        cons.cursor.col += x;
}

/*
 * Set the cursor an X amount of columns iff it does not exceed
 * cons.cols.
 */
static void __unused
cursor_column_set(int32_t x)
{

        cons.cursor.col = x;
}

/*
 * Set the cursor to COL and return TRUE iff the COL has not been
 * exceeded.
 */
static bool __unused
cursor_column_cond_set(int32_t col)
{

        if ((col >= 0) && (col <= cons.cols)) {
                cons.cursor.col = col;
                return true;
        }

        return false;
}

/*
 * Set both the COL and ROW of the cursor and return TRUE iff the COL
 * and ROW both have not been exceeded.
 */
static bool __unused
cursor_cond_set(int32_t col, int32_t row)
{

        if (((col >= 0) && (col <= cons.cols)) &&
            ((row >= 0) && (row <= cons.rows))) {
                cons.cursor.col = col;
                cons.cursor.row = row;
                return true;
        }

        return false;
}

static inline void
buffer_clear(int32_t col_start, int32_t col_end,
    int32_t row_start, int32_t row_end)
{
        int32_t col;
        int32_t row;

        for (row = row_start; row < row_end; row++) {
                for (col = col_start; col < col_end; col++) {
                        struct cons_buffer *cb;

                        cb = &cons.buffer[col + (row * cons.cols)];
                        cb->glyph = '\0';
                }
        }
}

static inline void
buffer_glyph_write(uint8_t glyph)
{
        struct cons_buffer *cb;

        cb = &cons.buffer[cons.cursor.col +
            (cons.cursor.row * cons.cols)];
        cb->glyph = glyph;
}

static inline uint8_t
buffer_attribute_read(void)
{
        struct cons_buffer *cb;

        cb = &cons.buffer[cons.cursor.col +
            (cons.cursor.row * cons.cols)];

        return cb->attribute;
}

static inline void
buffer_attribute_write(uint8_t attribute)
{
        struct cons_buffer *cb;

        cb = &cons.buffer[cons.cursor.col +
            (cons.cursor.row * cons.cols)];
        cb->attribute = attribute;
}

static inline void
buffer_fg_write(uint8_t fg)
{
        struct cons_buffer *cb;

        cb = &cons.buffer[cons.cursor.col +
            (cons.cursor.row * cons.cols)];
        cb->fg = fg;
}

static inline void
buffer_bg_write(uint8_t bg)
{
        struct cons_buffer *cb;

        cb = &cons.buffer[cons.cursor.col +
            (cons.cursor.row * cons.cols)];
        cb->bg = bg;
}

static inline void
action_character_print(int ch)
{
        if (cursor_column_exceeded(0)) {
                cursor_column_set(0);
                cursor_row_advance(1);
        }

        buffer_glyph_write(ch);
        cursor_column_advance(1);
}

static inline void
action_escape_character_print(int ch)
{
        int32_t tab;

        switch (ch) {
        case '\0':
        case '\a':
        case '\f':
                break;
        case '\b':
                if (!cursor_column_exceeded(-1)) {
                        cursor_column_advance(-1);
                }
                break;
        case '\n':
                cursor_column_set(0);
                cursor_row_advance(1);
                break;
        case '\r':
                cursor_column_set(0);
                break;
        case '\t':
                tab = CONS_TAB_WIDTH;

                if (cursor_column_exceeded(CONS_TAB_WIDTH)) {
                        if ((tab = (cons.cols - cons.cursor.col - 1)) < 0) {
                                break;
                        }
                }

                cursor_column_advance(tab);
                break;
        case '\v':
                cursor_row_advance(1);
                break;
        }
}

static inline void
action_csi_dispatch_print(int ch, int *params,
    int num_params)
{
        int32_t col;
        int32_t row;

        uint16_t ofs;

        switch (ch) {
        case 'A':
                /* ESC [ Pn A */

                /* A parameter value of zero or one moves the active
                 * position one line upward */
                if (num_params == 0) {
                        break;
                }

                row = (params[0] == 0) ? 1 : params[0];
                if (cursor_row_exceeded(-row)) {
                        row = cons.cursor.row;
                }

                cursor_row_advance(-row);
                break;
        case 'B':
                /* ESC [ Pn B */

                /* A parameter value of zero or one moves the active
                 * position one line downward */
                if (num_params == 0) {
                        break;
                }

                row = (params[0] == 0) ? 1 : params[0];
                if (cursor_row_exceeded(row)) {
                        row = cons.rows - cons.cursor.row - 1;
                }

                cursor_row_advance(row);
                break;
        case 'C':
                /* ESC [ Pn C */

                /* A parameter value of zero or one moves the active
                 * position one position to the right. A parameter value
                 * of n moves the active position n positions to the
                 * right */

                if (num_params == 0) {
                        break;
                }

                col = (params[0] == 0) ? 1 : params[0];
                if (cursor_column_exceeded(col)) {
                        col = cons.cols - cons.cursor.col - 1;
                }

                cursor_column_advance(col);
                break;
        case 'D':
                /* ESC [ Pn D */

                /* A parameter value of zero or one moves the active
                 * position one position to the left. A parameter value
                 * of n moves the active position n positions to the
                 * left */

                if (num_params == 0) {
                        break;
                }

                col = (params[0] == 0) ? 1 : params[0];
                if (cursor_column_exceeded(-col)) {
                        col = cons.cursor.col;
                }

                cursor_column_advance(-col);
                break;
        case 'H':
                /* ESC [ Pn ; Pn H */

                /* Both parameters are absolute: [0..cons.cols] and
                 * [0..cons.rows]. However, relative to the user, the values
                 * range from (0..cons.cols] and (0..cons.rows]. */

                /* This sequence has two parameter values, the first
                 * specifying the line position and the second
                 * specifying the column position */
                if (num_params == 0) {
                        cursor_column_set(0);
                        cursor_row_set(0);
                        break;
                }

                if (((num_params & 1) != 0) || (num_params > 2)) {
                        break;
                }

                col = ((params[1] - 1) < 0) ? 0 : params[1] - 1;
                row = ((params[0] - 1) < 0) ? 0 : params[0] - 1;

                cursor_cond_set(col, row);
                break;
        case 'J':
                /* ESC [ Ps J */
                if (num_params == 0) {
                        break;
                }

                switch (params[0]) {
                case 0:
                        /* Erase from the active position to the end of
                         * the screen, inclusive (default). */
                        buffer_clear(cons.cursor.col, cons.cols,
                            cons.cursor.row, cons.rows);
                        break;
                case 1:
                        /* Erase from start of the screen to the active
                         * position, inclusive. */
                        buffer_clear(0, cons.cursor.col, 0,
                            cons.cursor.row);
                        break;
                case 2:
                        /* Erase all of the display –- all lines are
                         * erased, changed to single-width, and the
                         * cursor does not move. */
                        buffer_clear(0, cons.cols, 0, cons.rows);
                        break;
                default:
                        break;
                }
                break;
        case 'K':
                /* ESC [ Ps K */
                if (num_params == 0) {
                        break;
                }

                switch (params[0]) {
                case 0:
                        /* Erase from the active position to the end of
                         * the line, inclusive (default) */
                        buffer_clear(cons.cursor.col, cons.cols,
                            cons.cursor.row, cons.cursor.row + 1);
                        break;
                case 1:
                        /* Erase from the start of the screen to the
                         * active position, inclusive */
                        buffer_clear(0, cons.cursor.col,
                            cons.cursor.row, cons.cursor.row + 1);
                        break;
                case 2:
                        /* Erase all of the line, inclusive */
                        buffer_clear(0, cons.cols,
                            cons.cursor.row, cons.cursor.row + 1);
                        break;
                default:
                        break;
                }
                break;
        case 'm':
                /* ESC [ Ps ; . . . ; Ps m */

                if ((num_params & 1) != 0) {
                        /* Number of parameters is odd */
                        break;
                }

                if (num_params == 0) {
                        buffer_fg_write(CONS_PALETTE_FG_WHITE);
                        buffer_bg_write(CONS_PALETTE_BG_BLACK);
                        break;
                }

                for (ofs = 0; ofs < num_params; ofs += 2) {
                        /* Attribute */
                        switch (params[ofs]) {
                        case CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES:
                                buffer_attribute_write(0);
                                break;
                        case CONS_ATTRIBUTE_BRIGHT:
                                buffer_attribute_write(8);
                                break;
                        case CONS_ATTRIBUTE_DIM:
                        case CONS_ATTRIBUTE_UNDERSCORE:
                        case CONS_ATTRIBUTE_BLINK:
                        case CONS_ATTRIBUTE_REVERSE:
                        case CONS_ATTRIBUTE_HIDDEN:
                        default:
                                buffer_attribute_write(CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES);
                                break;
                        }

                        /* Foreground & background */
                        switch (params[ofs + 1]) {
                        case CONS_PALETTE_FG_BLACK:
                        case CONS_PALETTE_FG_RED:
                        case CONS_PALETTE_FG_GREEN:
                        case CONS_PALETTE_FG_YELLOW:
                        case CONS_PALETTE_FG_BLUE:
                        case CONS_PALETTE_FG_MAGENTA:
                        case CONS_PALETTE_FG_CYAN:
                        case CONS_PALETTE_FG_WHITE:
                                buffer_fg_write(params[ofs + 1] + buffer_attribute_read());
                                break;
                        case CONS_PALETTE_BG_BLACK:
                        case CONS_PALETTE_BG_RED:
                        case CONS_PALETTE_BG_GREEN:
                        case CONS_PALETTE_BG_YELLOW:
                        case CONS_PALETTE_BG_BLUE:
                        case CONS_PALETTE_BG_MAGENTA:
                        case CONS_PALETTE_BG_CYAN:
                        case CONS_PALETTE_BG_WHITE:
                                buffer_bg_write(params[ofs + 1] + buffer_attribute_read());
                                break;
                        }
                }

                break;
        }
}
