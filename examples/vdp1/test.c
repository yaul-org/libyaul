/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/queue.h>

#include "test.h"

void (*test_update)(void) = test_nop;
void (*test_draw)(void) = test_nop;
void (*test_exit)(void) = test_nop;

TAILQ_HEAD(tests, test);

struct test {
        const char *name;
        void (*init)(void);
        void (*update)(void);
        void (*draw)(void);
        void (*exit)(void);

        TAILQ_ENTRY(test) entries;
};

static struct tests tests;
static bool initialized = false;

void
test_init(void)
{
        TAILQ_INIT(&tests);

        test_update = test_nop;
        test_draw = test_nop;
        test_exit = test_nop;

        initialized = true;
}

int
test_register(const char *name, void (*init)(void), void (*update)(void),
    void (*draw)(void), void (*exit)(void))
{
        assert(initialized);

        struct test *test;
        test = (struct test *)malloc(sizeof(struct test));
        assert(test != NULL);

        test->name = name;
        test->init = init;
        test->update = update;
        test->draw = draw;
        test->exit = exit;

        TAILQ_INSERT_TAIL(&tests, test, entries);

        return 0;
}

int
test_load(const char *name)
{
        assert(initialized);

        struct test *test;
        TAILQ_FOREACH (test, &tests, entries) {
                if ((strcmp(test->name, name)) == 0) {
                        test_exit();

                        test->init();

                        test_update = test->update;
                        test_draw = test->draw;
                        test_exit = test->exit;
                        return 0;
                }
        }

        return -1;
}

void
test_nop(void)
{
        assert(initialized);

        /* Do nothing */
}
