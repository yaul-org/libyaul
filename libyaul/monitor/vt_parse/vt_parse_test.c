#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/queue.h>

#include "vt_parse.h"

#define COLS 40
#define ROWS 28

typedef struct {
        struct {
                uint8_t x;
                uint8_t y;
        } cursor;
} terminal_t;

terminal_t term = {
        {
                .x = 0,
                .y = 0
        }
};

static bool cursor_columns_exceeded(terminal_t *, uint32_t);
static bool cursor_rows_exceeded(terminal_t *, uint32_t);
static void cursor_column_advance(terminal_t *, uint16_t);
static void cursor_newline_make(terminal_t *);
static void cursor_row_advance(terminal_t *, uint16_t);
static void terminal_reset(terminal_t *);

static int16_t alphabet_mapping[128] = {
        0
};

#define ALPHABET_LEN            5
#define PRODUCTIONS             5
#define PRODUCTION_SYMBOLS      2 /* Unique no. of productions */
#define STATES                  9

#define LR_ACTION(x, s, a)      ((x)[(s)].ac_action[alphabet_mapping[(a)]])
#define LR_GOTO(x, s, g)        ((x)[(s)].ac_goto[(g) - 1])

/* Return the production rule no. */
#define LR_PRODUCTION(m)        ((m) - STATES)

/* Total size of action + goto table
 * STATES * (sizeof(int16_t) * (ALPHABET_LEN + PRODUCTIONS)) */

typedef struct {
#define NIL             (-1)
#define S(n)            ((n))
#define R(m)            (STATES + (m))
#define ACC             (STATES + PRODUCTIONS + 1)
        int16_t ac_action[ALPHABET_LEN];
        int16_t ac_goto[PRODUCTIONS];
} action_t;

static action_t action_tbl[STATES] = {
        {
                /* STATE 0 */
                .ac_action = {
                        NIL, /* '*' */
                        NIL, /* '+' */
                        S(1), /* '0' */
                        S(2), /* '1' */
                        NIL /* $ */
                },
                .ac_goto = {
                        3, /* (1) E */
                        3, /* (2) E */
                        3, /* (3) E */
                        4, /* (4) B */
                        4 /* (5) B */
                }
        }, {
                /* STATE 1 */
                .ac_action = {
                        R(4), /* '*' */
                        R(4), /* '+' */
                        R(4), /* '0' */
                        R(4), /* '1' */
                        R(4), /* $ */
                },
                .ac_goto = {
                        NIL, /* (1) E */
                        NIL, /* (2) E */
                        NIL, /* (NIL) E */
                        NIL, /* (4) B */
                        NIL /* (5) B */
                }
        }, {
                /* STATE 2 */
                .ac_action = {
                        R(5), /* '*' */
                        R(5), /* '+' */
                        R(5), /* '0' */
                        R(5), /* '1' */
                        R(5), /* $ */
                },
                .ac_goto = {
                        NIL, /* (1) E */
                        NIL, /* (2) E */
                        NIL, /* (NIL) E */
                        NIL, /* (4) B */
                        NIL /* (5) B */
                }
        }, {
                /* STATE 3 */
                .ac_action = {
                        S(5), /* '*' */
                        S(6), /* '+' */
                        NIL, /* '0' */
                        NIL, /* '1' */
                        ACC, /* $ */
                },
                .ac_goto = {
                        NIL, /* (1) E */
                        NIL, /* (2) E */
                        NIL, /* (NIL) E */
                        NIL, /* (4) B */
                        NIL /* (5) B */
                }
        }, {
                /* STATE 4 */
                .ac_action = {
                        R(3), /* '*' */
                        R(3), /* '+' */
                        R(3), /* '0' */
                        R(3), /* '1' */
                        R(3), /* $ */
                },
                .ac_goto = {
                        NIL, /* (1) E */
                        NIL, /* (2) E */
                        NIL, /* (NIL) E */
                        NIL, /* (4) B */
                        NIL /* (5) B */
                }
        }, {
                /* STATE 5 */
                .ac_action = {
                        NIL, /* '*' */
                        NIL, /* '+' */
                        S(1), /* '0' */
                        S(2), /* '1' */
                        NIL, /* $ */
                },
                .ac_goto = {
                        NIL, /* (1) E */
                        NIL, /* (2) E */
                        NIL, /* (NIL) E */
                        7, /* (4) B */
                        7 /* (5) B */
                }
        }, {
                /* STATE 6 */
                .ac_action = {
                        NIL, /* '*' */
                        NIL, /* '+' */
                        S(1), /* '0' */
                        S(2), /* '1' */
                        NIL, /* $ */
                },
                .ac_goto = {
                        3, /* (1) E */
                        3, /* (2) E */
                        3, /* (NIL) E */
                        8, /* (4) B */
                        8 /* (5) B */
                }
        }, {
                /* STATE 7 */
                .ac_action = {
                        R(1), /* '*' */
                        R(1), /* '+' */
                        R(1), /* '0' */
                        R(1), /* '1' */
                        R(1), /* $ */
                },
                .ac_goto = {
                        NIL, /* (1) E */
                        NIL, /* (2) E */
                        NIL, /* (NIL) E */
                        NIL, /* (4) B */
                        NIL /* (5) B */
                }
        }, {
                /* STATE 8 */
                .ac_action = {
                        R(2), /* '*' */
                        R(2), /* '+' */
                        R(2), /* '0' */
                        R(2), /* '1' */
                        R(2), /* $ */
                },
                .ac_goto = {
                        NIL, /* (1) E */
                        NIL, /* (2) E */
                        NIL, /* (NIL) E */
                        NIL, /* (4) B */
                        NIL /* (5) B */
                }
        }
};

static char *
action2str(uint16_t action)
{
        return '\0';
}

void
parser_callback(vt_parse_t *parser, vt_parse_action_t action, uint8_t ch)
{
        int i;

        /*
         * start : '^[' csi print
         *       | print
         * print : '\' special-character print
         *       | normal-character print
         *       | escape print
         *       | print
         *       | \/* empty *\/
         * special-character : 'a' 'b' 't' 'n' 'v' 'f' 'r' '0'
         * normal-character : [^\\]
         * escape : '\' '\' escape
         *        | '\' '\' normal-character
         *        | \/* empty *\/
         * csi : '['
         */

        switch (action) {
        case VT_PARSE_ACTION_PRINT:
                /* putc(ch, stdout); */
                (void)printf("0x%02X\n", ch);
        case VT_PARSE_ACTION_EXECUTE:
                fflush(stdout);
                break;
        case VT_PARSE_ACTION_CSI_DISPATCH:
                (void)printf("'%s': %d Parameters\n", parser->intermediate_chars,
                    parser->num_params);
                for(i = 0; i < parser->num_params; i++)
                        (void)printf("-> %i\n", parser->params[i]);
                (void)printf("%c\n", ch);
                break;
        }
}

typedef TAILQ_HEAD(lifo, entry) lifo_t;

typedef struct entry {
        int16_t state;
        TAILQ_ENTRY(entry) entries;
} entry_t;

static void
pop(lifo_t *top)
{
        entry_t *top_entry;

        assert(top != NULL);

        if (TAILQ_EMPTY(top))
                return;

        top_entry = TAILQ_FIRST(top);
        TAILQ_REMOVE(top, top_entry, entries);

        free(top_entry);
}

static entry_t *
top(lifo_t *top)
{
        entry_t *top_entry;

        top_entry = NULL;
        if (!TAILQ_EMPTY(top))
                top_entry = TAILQ_FIRST(top);

        return top_entry;
}

static void
push(lifo_t *top, int16_t state)
{
        entry_t *entry;

        assert(top != NULL);

        if ((entry = (entry_t *)malloc(sizeof(entry_t))) == NULL)
                return;

        entry->state = state;
        TAILQ_INSERT_HEAD(top, entry, entries);
}

static void
print(lifo_t *top)
{
        entry_t *np;

        assert(top != NULL);

        (void)printf("==> stack ");
        TAILQ_FOREACH(np, top, entries) {
                (void)printf("%i ", np->state);
        }
        (void)printf("\n");
}

static void
lifo(lifo_t *top)
{

        TAILQ_INIT(top);
}

static void
parser_lr_0_action(int16_t prod, char ch)
{
        switch (prod) {
        default:
                (void)printf("========================================> %i '%c'\n",
                    prod, ch);
                break;
        }
}

/* LR(0) bottom-up/shift-reduce parser */
static void
parser_lr_0(char *input_stream)
{
        lifo_t lr_lifo;
        entry_t *np;

        int16_t state;
        int16_t action;

        lifo(&lr_lifo);

        (void)printf("|action_tbl| = %luB\n", sizeof(action_tbl));

        while (true) {
                if (state == NIL) {
                        (void)fprintf(stderr, "Invalid input stream\n");
                        return;
                }

                (void)printf("'%s'\n", input_stream);
                if ((top(&lr_lifo)) == NULL) {
                        state = 0;
                        push(&lr_lifo, state);
                } else {
                        np = top(&lr_lifo);
                        state = np->state;
                }

                action = LR_ACTION(action_tbl, state, *input_stream);

                if (action == ACC) {
                        print(&lr_lifo);
                        (void)printf("Input stream: \"%s\"\n", input_stream);
                        pop(&lr_lifo);
                        return;
                }

                if (action == NIL)
                        return;

                if ((action >= R(1)) && (action <= R(PRODUCTIONS))) {
                        print(&lr_lifo);

                        printf("state == %i\n", state);
                        pop(&lr_lifo);
                        np = top(&lr_lifo);
                        state = np->state;

                        print(&lr_lifo);

                        state = LR_GOTO(action_tbl, state, LR_PRODUCTION(action));
                        (void)printf("==> reduce(%i) goto state(%i)\n",
                            LR_PRODUCTION(action),
                            state);

                        /* Special case? */
                        if (*input_stream == '\0') {
                                while ((np = top(&lr_lifo)) != NULL) {
                                        if (np->state == 0)
                                                break;
                                        pop(&lr_lifo);
                                }
                        }

                        push(&lr_lifo, state);
                }

                if ((action >= S(1)) && (action <= S(STATES - 1))) {
                        state = action;
                        push(&lr_lifo, state);
                        (void)printf("==> state(%i)\n", top(&lr_lifo)->state);

                        /* Perform production rule action */
                        parser_lr_0_action(state, *input_stream);
                        input_stream++; /* Shift */
                }
        }
}

int
main(int argc, char *argv[])
{
    unsigned char buf[1024];
    int bytes;
    vt_parse_t parser;

    alphabet_mapping['*'] = 0;
    alphabet_mapping['+'] = 1;
    alphabet_mapping['0'] = 2;
    alphabet_mapping['1'] = 3;
    alphabet_mapping['\0'] = 4; /* $ symbol */

    /* vt_parse_init(&parser, parser_callback); */

    parser_lr_0("1+0");

    /* while(1) { */
    /*     bytes = read(0, buf, 1024); */
    /*     vt_parse(&parser, buf, bytes); */
    /* } */
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
