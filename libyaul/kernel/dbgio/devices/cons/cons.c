/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cons.h"

#include "vt_parse.inc"

static void _action_character_print(int ch);
static void _action_escape_character_print(int ch);
static void _action_csi_dispatch_print(int8_t ch, const uint8_t *params,
    uint32_t num_params);
static inline bool _cursor_column_exceeded(int16_t x) __always_inline;
static inline bool _cursor_row_exceeded(uint16_t y) __always_inline;
static inline void _cursor_column_advance(int16_t x) __always_inline;
static inline void _cursor_column_set(int16_t x) __always_inline;
static inline void _cursor_row_advance(uint16_t y) __always_inline;
static inline bool _cursor_row_cond_set(int16_t row) __always_inline;
static inline void _cursor_row_set(int16_t y) __always_inline;

static void _vt_parser_callback(vt_parse_t *parser, vt_parse_action_t action,
    int ch);

typedef struct {
        uint16_t cols;
        uint16_t rows;

        struct {
                int16_t col;
                int16_t row;
        } cursor;

        cons_ops_t ops;

        vt_parse_t vt_parser;
} cons_t;

static cons_t _cons;

void
cons_init(const cons_ops_t *ops, uint16_t cols, uint16_t rows)
{
        assert(ops != NULL);

        _cons.ops.clear = ops->clear;
        _cons.ops.area_clear = ops->area_clear;
        _cons.ops.line_clear = ops->line_clear;
        _cons.ops.write = ops->write;

        assert(_cons.ops.clear != NULL);
        assert(_cons.ops.area_clear != NULL);
        assert(_cons.ops.line_clear != NULL);
        assert(_cons.ops.write != NULL);

        _cons.cols = 0;
        _cons.rows = 0;

        _cons.cursor.col = 0;
        _cons.cursor.row = 0;

        cons_resize(cols, rows);

        ops->clear();

        _vt_parse_init(&_cons.vt_parser, _vt_parser_callback);
}

void
cons_resize(uint16_t cols, uint16_t rows)
{
        assert((cols >= CONS_COLS_MIN) && (cols <= CONS_COLS_MAX));
        assert((rows >= CONS_ROWS_MIN) && (rows <= CONS_ROWS_MAX));

        _cons.cols = cols;
        _cons.rows = rows;

        _cons.cursor.col = clamp(_cons.cursor.col, 0, cols - 1);
        _cons.cursor.row = clamp(_cons.cursor.row, 0, rows - 1);
}

void
cons_buffer(const char *buffer)
{
        assert(buffer != NULL);

        size_t len;

        if (*buffer == '\0') {
                return;
        }

        if ((len = strlen(buffer)) == 0) {
                return;
        }

        _vt_parse(&_cons.vt_parser, buffer, len);
}

static void
_vt_parser_callback(vt_parse_t *parser, vt_parse_action_t action, int ch)
{
        switch (action) {
        case VT_PARSE_ACTION_PRINT:
        case VT_PARSE_ACTION_IGNORE:
        case VT_PARSE_ACTION_INVALID:
                _action_character_print(ch);
                break;
        case VT_PARSE_ACTION_EXECUTE:
                _action_escape_character_print(ch);
                break;
        case VT_PARSE_ACTION_CSI_DISPATCH:
                _action_csi_dispatch_print(ch, &parser->params[0],
                    parser->num_params);
                break;
        default:
                break;
        }
}

/*
 * Return true if the current cursor position plus an X amount of columns is out
 * of bounds.
 */
static inline bool __always_inline
_cursor_column_exceeded(int16_t x)
{
        const int16_t col = _cons.cursor.col + x;

        return (col < 0) || (col >= _cons.cols);
}

/*
 * Return true if the current cursor position plus an Y amount of rows is out of
 * bounds.
 */
static inline bool __always_inline
_cursor_row_exceeded(uint16_t y)
{
        const int16_t row = _cons.cursor.row + y;

        return (row < 0) || (row >= _cons.rows);
}

/*
 * Advance the cursor an Y amount of rows.
 */
static inline void __always_inline
_cursor_row_advance(uint16_t y)
{
        _cons.cursor.row += y;
}

/*
 * Set the cursor an Y amount of rows.
 */
static inline void __always_inline
_cursor_row_set(int16_t y)
{
        _cons.cursor.row = y;
}

/*
 * Set the cursor to ROW and return TRUE iff the ROW has not been exceeded.
 */
static inline bool __always_inline
_cursor_row_cond_set(int16_t row)
{
        if ((row >= 0) && (row <= _cons.rows)) {
                _cons.cursor.row = row;
                return true;
        }

        return false;
}

/*
 * Advance the cursor an X amount of columns.
 */
static inline void __always_inline
_cursor_column_advance(int16_t x)
{
        _cons.cursor.col += x;
}

/*
 * Set the cursor an X amount of columns iff it does not exceed _cons.cols.
 */
static inline void __always_inline
_cursor_column_set(int16_t x)
{
        _cons.cursor.col = x;
}

static void
_action_character_print(int ch)
{
        if (_cursor_column_exceeded(0)) {
                _cursor_column_set(0);
                _cursor_row_advance(1);
        }

        if (_cursor_row_exceeded(0)) {
                return;
        }

        _cons.ops.write(_cons.cursor.col, _cons.cursor.row, ch);

        _cursor_column_advance(1);
}

static void
_action_escape_character_print(int ch)
{
        int16_t tab;

        switch (ch) {
        case '\0':
        case '\a':
        case '\f':
                break;
        case '\b':
                if (!_cursor_column_exceeded(-1)) {
                        _cursor_column_advance(-1);
                }
                break;
        case '\n':
                _cursor_column_set(0);
                _cursor_row_advance(1);
                break;
        case '\r':
                _cursor_column_set(0);
                break;
        case '\t':
                tab = CONS_TAB_WIDTH;

                if (_cursor_column_exceeded(CONS_TAB_WIDTH)) {
                        if ((tab = (_cons.cols - _cons.cursor.col - 1)) < 0) {
                                break;
                        }
                }

                _cursor_column_advance(tab);
                break;
        case '\v':
                _cursor_row_advance(1);
                break;
        default:
                _action_character_print(ch);
                break;
        }
}

static void
_action_csi_a(const uint8_t *params, const uint8_t num_params)
{
        /* A parameter value of zero or one moves the active
         * position one line upward */
        if (num_params == 0) {
                return;
        }

        int16_t row;
        row = (params[0] == 0) ? 1 : params[0];

        if (_cursor_row_exceeded(-row)) {
                row = _cons.cursor.row;
        }

        _cursor_row_advance(-row);
}


static void
_action_csi_b(const uint8_t *params, const uint8_t num_params)
{
        /* A parameter value of zero or one moves the active position one line
         * downward */
        if (num_params == 0) {
                return;
        }

        int16_t row;
        row = (params[0] == 0) ? 1 : params[0];

        if (_cursor_row_exceeded(row)) {
                row = _cons.rows - _cons.cursor.row - 1;
        }

        _cursor_row_advance(row);
}

static void
_action_csi_c(const uint8_t *params, const uint8_t num_params)
{
        /* A parameter value of zero or one moves the active position one
         * position to the right. A parameter value of n moves the active
         * position n positions to the right */

        if (num_params == 0) {
                return;
        }

        int16_t col;
        col = (params[0] == 0) ? 1 : params[0];

        if (_cursor_column_exceeded(col)) {
                col = _cons.cols - _cons.cursor.col - 1;
        }

        _cursor_column_advance(col);
}

static void
_action_csi_d(const uint8_t *params, const uint8_t num_params)
{
        /* A parameter value of zero or one moves the active position one
         * position to the left. A parameter value of n moves the active
         * position n positions to the left */

        if (num_params == 0) {
                return;
        }

        int16_t col;
        col = (params[0] == 0) ? 1 : params[0];

        if (_cursor_column_exceeded(-col)) {
                col = _cons.cursor.col;
        }

        _cursor_column_advance(-col);
}

static void
_action_csi_h(const uint8_t *params, const uint8_t num_params)
{
        /* Both parameters are absolute: [0.._cons.cols] and [0.._cons.rows].
         * However, relative to the user, the values range from (0.._cons.cols]
         * and (0.._cons.rows]. */

        /* This sequence has two parameter values, the first specifying the line
         * position and the second specifying the column position */
        if (num_params == 0) {
                _cursor_column_set(0);
                _cursor_row_set(0);

                return;
        }

        if (((num_params & 1) != 0) || (num_params > 2)) {
                return;
        }

        int16_t col;
        col = ((params[1] - 1) < 0) ? 0 : params[1] - 1;
        int16_t row;
        row = ((params[0] - 1) < 0) ? 0 : params[0] - 1;

        /* Don't bother clamping */
        _cursor_column_set(col);
        _cursor_row_set(row);
}

static void
_action_csi_j(const uint8_t *params, const uint8_t num_params)
{
        int16_t col;
        col = _cons.cursor.col;
        int16_t row;
        row = _cons.cursor.row;

        uint8_t number;
        number = (num_params > 0) ? params[0] : 0;

        switch (number) {
        case 0:
                /* Erase from the active position to the end of the screen,
                 * inclusive (default). */

                /* If the cursor is at the top left, just clear the entire
                 * screen */
                if ((col == 0) && (row == 0)) {
                        _cons.ops.clear();
                        break;
                }

                /* If the column or row difference is zero, don't erase */
                if (((_cons.cols - col) <= 0) ||
                    ((_cons.rows - row) <= 0)) {
                        break;
                }

                _cons.ops.area_clear(col, _cons.cols, row, _cons.rows);
                break;
        case 1:
                /* Erase from start of the screen to the active position,
                 * inclusive. */
                if ((col == 0) || (row == 0)) {
                        break;
                }

                if (_cursor_column_exceeded(0)) {
                        col = _cons.cols;
                }

                if (_cursor_row_exceeded(0)) {
                        row = _cons.rows;
                }

                _cons.ops.area_clear(0, col, 0, row);
                break;
        case 2:
                /* Erase all of the display -- all lines are erased, changed to
                 * single-width, and the cursor does not move. */
                _cons.ops.clear();
                break;
        default:
                break;
        }
}

static void
_action_csi_k(const uint8_t *params, const uint8_t num_params)
{
        if (_cursor_row_exceeded(0)) {
                return;
        }

        uint8_t number;
        number = (num_params > 0) ? params[0] : 0;

        int16_t col;
        col = _cons.cursor.col;
        int16_t row;
        row = _cons.cursor.row;

        switch (number) {
        case 0:
                /* Erase from the active position to the end of the line,
                 * inclusive (default) */

                /* If the column is zero, or exceeded screen, don't erase */
                if ((_cons.cols - col) <= 0) {
                        break;
                }

                if (col == 0) {
                        _cons.ops.line_clear(row);
                } else {
                        _cons.ops.line_partial_clear(col, _cons.cols, row);
                }
                break;
        case 1:
                /* Erase from the start of the screen to the active position,
                 * inclusive */
                if (col == 0) {
                        break;
                }

                col = (_cursor_column_exceeded(0)) ? _cons.cols : col;

                _cons.ops.line_partial_clear(0, col, row);
                break;
        case 2:
                /* Erase all of the line, inclusive */
                _cons.ops.line_clear(row);
                break;
        default:
                break;
        }
}

static void
_action_csi_dispatch_print(int8_t ch, const uint8_t *params, uint32_t num_params)
{
        switch (ch) {
        case 'A':
                /* ESC [ Pn A */
                _action_csi_a(params, num_params);
                break;
        case 'B':
                /* ESC [ Pn B */
                _action_csi_b(params, num_params);
                break;
        case 'C':
                /* ESC [ Pn C */
                _action_csi_c(params, num_params);
                break;
        case 'D':
                /* ESC [ Pn D */
                _action_csi_d(params, num_params);
                break;
        case 'H':
                /* ESC [ Pn ; Pn H */
                _action_csi_h(params, num_params);
                break;
        case 'J':
                /* ESC [ Ps J */
                _action_csi_j(params, num_params);
                break;
        case 'K':
                /* ESC [ Ps K */
                _action_csi_k(params, num_params);
                break;
        case 'm':
                /* ESC [ Ps ; . . . ; Ps m */
                break;
        }
}
