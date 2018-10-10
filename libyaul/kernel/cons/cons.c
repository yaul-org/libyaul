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
#include "drivers.h"

#include "vt_parse/vt_parse.h"

static inline __attribute__ ((always_inline)) void _action_character_print(int);
static inline __attribute__ ((always_inline)) void _action_escape_character_print(int);
static inline __attribute__ ((always_inline)) void _action_csi_dispatch_print(int32_t, int32_t *, int32_t);

static void _buffer_clear(int32_t, int32_t, int32_t, int32_t);
static void _buffer_line_clear(int32_t, int32_t, int32_t);
static inline void __attribute__ ((always_inline)) _buffer_glyph_write(uint8_t);

static inline __attribute__ ((always_inline)) bool _cursor_column_exceeded(int32_t);
static inline __attribute__ ((always_inline)) bool _cursor_row_exceeded(uint32_t);
static inline __attribute__ ((always_inline)) void _cursor_column_advance(int32_t);
static inline __attribute__ ((always_inline)) bool _cursor_column_cond_set(int32_t);
static inline __attribute__ ((always_inline)) void _cursor_column_set(int32_t);
static inline __attribute__ ((always_inline)) void _cursor_row_advance(uint16_t);
static inline __attribute__ ((always_inline)) bool _cursor_row_cond_set(int32_t);
static inline __attribute__ ((always_inline)) void _cursor_row_set(int32_t);
static inline __attribute__ ((always_inline)) bool _cursor_cond_set(int32_t, int32_t);

static void _vt_parser_callback(vt_parse_t *, vt_parse_action_t, int);

static struct cons _cons;
static vt_parse_t _vt_parser;

void
cons_init(uint8_t driver, uint16_t cols, uint16_t rows)
{
        assert(cols > 0);
        assert(rows > 0);

        if (_cons.buffer != NULL) {
                free(_cons.buffer);
        }

        _cons.buffer = malloc(cols * rows);

        assert(_cons.buffer != NULL);

        _cons.cols = cols;
        _cons.rows = rows;

        _cons.cursor.col = 0;
        _cons.cursor.row = 0;

        _buffer_clear(0, _cons.cols, 0, _cons.rows);

        _internal_vt_parse_init(&_vt_parser, _vt_parser_callback);

        switch (driver) {
        case CONS_DRIVER_VDP1:
                cons_vdp1_init(&_cons);
                break;
        case CONS_DRIVER_VDP2:
                cons_vdp2_init(&_cons);
                break;
        case CONS_DRIVER_USB_CARTRIDGE:
                cons_usb_cartridge_init(&_cons);
                break;
        default:
                assert(false);
                return;
        }
}

void
cons_buffer(const char *buffer)
{
        assert(buffer != NULL);

        size_t len;

        if ((len = strlen(buffer)) == 0) {
                return;
        }

        _internal_vt_parse(&_vt_parser, buffer, len);
}

void
cons_write(const char *buffer)
{
        assert(buffer != NULL);

        size_t len;
        if ((len = strlen(buffer)) == 0) {
                return;
        }

        _internal_vt_parse(&_vt_parser, buffer, len);

        _cons.write(&_cons);
}

void
cons_flush(void)
{
        _cons.write(&_cons);
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
static inline bool __attribute__ ((always_inline))
_cursor_column_exceeded(int32_t x)
{
        int32_t col;

        col = _cons.cursor.col + x;
        return (col < 0) || (col >= _cons.cols);
}

/*
 * Return true if the current cursor position plus an Y amount of rows
 * is out of bounds.
 */
static inline bool __attribute__ ((always_inline))
_cursor_row_exceeded(uint32_t y)
{
        int32_t row;

        row = _cons.cursor.row + y;
        return (row < 0) || (row >= _cons.rows);
}

/*
 * Advance the cursor an Y amount of rows.
 */
static inline void __attribute__ ((always_inline))
_cursor_row_advance(uint16_t y)
{
        _cons.cursor.row += y;
}

/*
 * Set the cursor an Y amount of rows.
 */
static inline void __attribute__ ((always_inline))
_cursor_row_set(int32_t y)
{
        _cons.cursor.row = y;
}

/*
 * Set the cursor to ROW and return TRUE iff the ROW has not been
 * exceeded.
 */
static inline bool __attribute__ ((always_inline))
_cursor_row_cond_set(int32_t row)
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
static inline void __attribute__ ((always_inline))
_cursor_column_advance(int32_t x)
{
        _cons.cursor.col += x;
}

/*
 * Set the cursor an X amount of columns iff it does not exceed
 * _cons.cols.
 */
static inline void __attribute__ ((always_inline))
_cursor_column_set(int32_t x)
{
        _cons.cursor.col = x;
}

/*
 * Set the cursor to COL and return TRUE iff the COL has not been
 * exceeded.
 */
static inline bool __attribute__ ((always_inline))
_cursor_column_cond_set(int32_t col)
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
static inline bool __attribute__ ((always_inline))
_cursor_cond_set(int32_t col, int32_t row)
{
        if (((col >= 0) && (col <= _cons.cols)) &&
            ((row >= 0) && (row <= _cons.rows))) {
                _cons.cursor.col = col;
                _cons.cursor.row = row;

                return true;
        }

        return false;
}

static void
_buffer_clear(int32_t col_start, int32_t col_end, int32_t row_start,
    int32_t row_end)
{
        int32_t col;
        int32_t row;

        for (row = row_start; row < row_end; row++) {
                for (col = col_start; col < col_end; col++) {
                        struct cons_buffer *cb;
                        cb = &_cons.buffer[col + (row * _cons.cols)];

                        cb->glyph = '\0';
                }
        }
}

static void
_buffer_line_clear(int32_t col_start, int32_t col_end, int32_t row)
{
        int32_t col;

        for (col = col_start; col < col_end; col++) {
                struct cons_buffer *cb;
                cb = &_cons.buffer[col + (row * _cons.cols)];

                cb->glyph = '\0';
        }
}

static inline void __attribute__ ((always_inline))
_buffer_glyph_write(uint8_t glyph)
{
        struct cons_buffer *cb;
        cb = &_cons.buffer[_cons.cursor.col + (_cons.cursor.row * _cons.cols)];

        cb->glyph = glyph;
}

static inline void __attribute__ ((always_inline))
_action_character_print(int ch)
{
        if (_cursor_column_exceeded(0)) {
                _cursor_column_set(0);
                _cursor_row_advance(1);
        }

        _buffer_glyph_write(ch);
        _cursor_column_advance(1);
}

static inline void __attribute__ ((always_inline))
_action_escape_character_print(int ch)
{
        int32_t tab;

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

static inline void __attribute__ ((always_inline))
_action_csi_dispatch_print(int32_t ch, int32_t *params, int32_t num_params)
{
        int32_t col;
        int32_t row;

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
                        _buffer_clear(_cons.cursor.col, _cons.cols,
                            _cons.cursor.row, _cons.rows);
                        break;
                case 1:
                        /* Erase from start of the screen to the active
                         * position, inclusive. */
                        _buffer_clear(0, _cons.cursor.col, 0,
                            _cons.cursor.row);
                        break;
                case 2:
                        /* Erase all of the display –- all lines are
                         * erased, changed to single-width, and the
                         * cursor does not move. */
                        _buffer_clear(0, _cons.cols, 0, _cons.rows);
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
                        _buffer_line_clear(_cons.cursor.col, _cons.cols,
                            _cons.cursor.row);
                        break;
                case 1:
                        /* Erase from the start of the screen to the
                         * active position, inclusive */
                        _buffer_line_clear(0, _cons.cursor.col,
                            _cons.cursor.row);
                        break;
                case 2:
                        /* Erase all of the line, inclusive */
                        _buffer_line_clear(0, _cons.cols, _cons.cursor.row);
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
