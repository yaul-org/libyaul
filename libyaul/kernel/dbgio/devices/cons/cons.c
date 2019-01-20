/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cons.h"

#include "vt_parse.inc"

static inline __always_inline void _action_character_print(int);
static inline __always_inline void _action_escape_character_print(int);
static inline __always_inline void _action_csi_dispatch_print(int16_t, int16_t *, int16_t);

static inline __always_inline bool _cursor_column_exceeded(int16_t);
static inline __always_inline bool _cursor_row_exceeded(uint16_t);
static inline __always_inline void _cursor_column_advance(int16_t);
static inline __always_inline bool _cursor_column_cond_set(int16_t);
static inline __always_inline void _cursor_column_set(int16_t);
static inline __always_inline void _cursor_row_advance(uint16_t);
static inline __always_inline bool _cursor_row_cond_set(int16_t);
static inline __always_inline void _cursor_row_set(int16_t);
static inline __always_inline bool _cursor_cond_set(int16_t, int16_t);

static void _vt_parser_callback(vt_parse_t *, vt_parse_action_t, int);

typedef struct {
        uint16_t cols;
        uint16_t rows;
        uint16_t cell_count;

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

        assert((cols >= CONS_COLS_MIN) && (cols <= CONS_COLS_MAX));
        assert((rows >= CONS_ROWS_MIN) && (rows <= CONS_ROWS_MAX));

        _cons.ops.clear = ops->clear;
        _cons.ops.area_clear = ops->area_clear;
        _cons.ops.line_clear = ops->line_clear;
        _cons.ops.write = ops->write;

        assert(_cons.ops.clear != NULL);
        assert(_cons.ops.area_clear != NULL);
        assert(_cons.ops.line_clear != NULL);
        assert(_cons.ops.write != NULL);

        _cons.cols = cols;
        _cons.rows = rows;
        _cons.cell_count = cols * rows;

        _cons.cursor.col = 0;
        _cons.cursor.row = 0;

        ops->clear();

        _vt_parse_init(&_cons.vt_parser, _vt_parser_callback);
}

void
cons_buffer(const char *buffer)
{
        assert(buffer != NULL);

        size_t len;

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
 * Return true if the current cursor position plus an X amount of
 * columns is out of bounds.
 */
static inline bool __always_inline
_cursor_column_exceeded(int16_t x)
{
        int16_t col;

        col = _cons.cursor.col + x;
        return (col < 0) || (col >= _cons.cols);
}

/*
 * Return true if the current cursor position plus an Y amount of rows
 * is out of bounds.
 */
static inline bool __always_inline
_cursor_row_exceeded(uint16_t y)
{
        int16_t row;

        row = _cons.cursor.row + y;
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
 * Set the cursor to ROW and return TRUE iff the ROW has not been
 * exceeded.
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
 * Set the cursor an X amount of columns iff it does not exceed
 * _cons.cols.
 */
static inline void __always_inline
_cursor_column_set(int16_t x)
{
        _cons.cursor.col = x;
}

/*
 * Set the cursor to COL and return TRUE iff the COL has not been
 * exceeded.
 */
static inline bool __always_inline
_cursor_column_cond_set(int16_t col)
{
        if ((col >= 0) && (col <= _cons.cols)) {
                _cons.cursor.col = col;
                return true;
        }

        return false;
}

/*
 * Set both the COL and ROW of the cursor and return TRUE iff the COL
 * and ROW both have not been exceeded.
 */
static inline bool __always_inline
_cursor_cond_set(int16_t col, int16_t row)
{
        if (((col >= 0) && (col <= _cons.cols)) &&
            ((row >= 0) && (row <= _cons.rows))) {
                _cons.cursor.col = col;
                _cons.cursor.row = row;

                return true;
        }

        return false;
}

static inline void __always_inline
_action_character_print(int ch)
{
        if (_cursor_column_exceeded(0)) {
                _cursor_column_set(0);
                _cursor_row_advance(1);
        }

        _cons.ops.write(_cons.cursor.col, _cons.cursor.row, ch);
        _cursor_column_advance(1);
}

static inline void __always_inline
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
        }
}

static inline void __always_inline
_action_csi_dispatch_print(int16_t ch, int16_t *params, int16_t num_params)
{
        int16_t col;
        int16_t row;

        switch (ch) {
        case 'A':
                /* ESC [ Pn A */

                /* A parameter value of zero or one moves the active
                 * position one line upward */
                if (num_params == 0) {
                        break;
                }

                row = (params[0] == 0) ? 1 : params[0];
                if (_cursor_row_exceeded(-row)) {
                        row = _cons.cursor.row;
                }

                _cursor_row_advance(-row);
                break;
        case 'B':
                /* ESC [ Pn B */

                /* A parameter value of zero or one moves the active
                 * position one line downward */
                if (num_params == 0) {
                        break;
                }

                row = (params[0] == 0) ? 1 : params[0];
                if (_cursor_row_exceeded(row)) {
                        row = _cons.rows - _cons.cursor.row - 1;
                }

                _cursor_row_advance(row);
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
                if (_cursor_column_exceeded(col)) {
                        col = _cons.cols - _cons.cursor.col - 1;
                }

                _cursor_column_advance(col);
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
                if (_cursor_column_exceeded(-col)) {
                        col = _cons.cursor.col;
                }

                _cursor_column_advance(-col);
                break;
        case 'H':
                /* ESC [ Pn ; Pn H */

                /* Both parameters are absolute: [0.._cons.cols] and
                 * [0.._cons.rows]. However, relative to the user, the values
                 * range from (0.._cons.cols] and (0.._cons.rows]. */

                /* This sequence has two parameter values, the first
                 * specifying the line position and the second
                 * specifying the column position */
                if (num_params == 0) {
                        _cursor_column_set(0);
                        _cursor_row_set(0);
                        break;
                }

                if (((num_params & 1) != 0) || (num_params > 2)) {
                        break;
                }

                col = ((params[1] - 1) < 0) ? 0 : params[1] - 1;
                row = ((params[0] - 1) < 0) ? 0 : params[0] - 1;

                _cursor_cond_set(col, row);
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
                        _cons.ops.area_clear(_cons.cursor.col, _cons.cols,
                            _cons.cursor.row, _cons.rows);
                        break;
                case 1:
                        /* Erase from start of the screen to the active
                         * position, inclusive. */
                        _cons.ops.area_clear(0, _cons.cursor.col, 0,
                            _cons.cursor.row);
                        break;
                case 2:
                        /* Erase all of the display –- all lines are
                         * erased, changed to single-width, and the
                         * cursor does not move. */
                        _cons.ops.clear();
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
                        _cons.ops.line_clear(_cons.cursor.col, _cons.cols,
                            _cons.cursor.row);
                        break;
                case 1:
                        /* Erase from the start of the screen to the
                         * active position, inclusive */
                        _cons.ops.line_clear(0, _cons.cursor.col,
                            _cons.cursor.row);
                        break;
                case 2:
                        /* Erase all of the line, inclusive */
                        _cons.ops.line_clear(0, _cons.cols, _cons.cursor.row);
                        break;
                default:
                        break;
                }
                break;
        case 'm':
                /* ESC [ Ps ; . . . ; Ps m */
                break;
        }
}
