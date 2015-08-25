/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef TESTS_H
#define TESTS_H

struct test {
        char *name;
        void (*init)(void);
        void (*update)(void);
        void (*draw)(void);
        void (*exit)(void);
};

#define TEST_TYPE_PRIMITIVES            0
#define TEST_TYPE_COLOR                 1
#define TEST_TYPE_SPECIAL_FUNCTIONS     2

extern struct test *tests[];

#endif /* !TESTS_H */
