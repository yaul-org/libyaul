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

#define TEST_SUBTYPE_NORMAL_SPRITE      0
#define TEST_SUBTYPE_SCALED_SPRITE      1
#define TEST_SUBTYPE_DISTORTED_SPRITE   2

const struct test *tests_fetch(uint32_t, uint32_t, uint32_t);

#endif /* !TESTS_H */
