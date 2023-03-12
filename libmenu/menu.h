#ifndef _MENU_H_
#define _MENU_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>

#define MENU_ACTION_ENTRY(_label, _action) {                                   \
        .label        = _label,                                                \
        .label_format = NULL,                                                  \
        .action_fn    = _action,                                               \
        .update_fn    = NULL,                                                  \
        .cycle_fn     = NULL                                                   \
}

#define MENU_CYCLE_ENTRY(_label_format, _cycle, _update) {                     \
        .label        = "",                                                    \
        .label_format = _label_format,                                         \
        .action_fn    = NULL,                                                  \
        .update_fn    = _update,                                               \
        .cycle_fn     = _cycle                                                 \
}

#define MENU_ACTION_CYCLE_ENTRY(_label_format, _action, _cycle, _update) {     \
        .label        = "",                                                    \
        .label_format = _label_format,                                         \
        .action_fn    = _action,                                               \
        .update_fn    = _update,                                               \
        .cycle_fn     = _cycle                                                 \
}

typedef int8_t menu_cursor_t;
typedef struct menu menu_t;
typedef struct menu_entry menu_entry_t;

typedef void (*menu_action_t)(void *work, menu_entry_t *menu_entry);
typedef void (*menu_cycle_t)(void *work, menu_entry_t *menu_entry, int32_t direction);
typedef menu_action_t menu_update_t;

typedef void (*menu_input_fn_t)(menu_t *menu);

struct menu_entry {
        char label[48];
        char *label_format;
        menu_action_t action_fn;
        menu_cycle_t cycle_fn;
        menu_update_t update_fn;
};

static_assert(sizeof(struct menu_entry) == 64);

#define MENU_MASK (0x0003)

typedef enum {
        MENU_NONE          = 0,
        MENU_ENABLED       = 1 << 0,
        MENU_INPUT_ENABLED = 1 << 1
} menu_flags_t;

struct menu {
        menu_entry_t *current_entry;
        menu_entry_t *entries;
        menu_flags_t flags;
        void *data;

        /* Private */
        menu_cursor_t _cursor;
        uint16_t _entries_count;
        menu_input_fn_t _input_fn;
};

void menu_init(menu_t *menu);

void menu_entries_set(menu_t *menu, menu_entry_t *entries, uint32_t count);
void menu_input_set(menu_t *menu, menu_input_fn_t input_fn);

void menu_update(menu_t *menu);

menu_cursor_t menu_cursor(menu_t *menu);
void menu_cursor_up_move(menu_t *menu);
void menu_cursor_down_move(menu_t *menu);
void menu_cycle_call(menu_t *menu, int32_t direction);
void menu_action_call(menu_t *menu);

#endif /* _MENU_H_ */
