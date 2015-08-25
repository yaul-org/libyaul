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

#include <common.h>

#include "tests.h"

#define TEST_PROTOTYPE_DECLARE(name)                                              \
    void CC_CONCAT(CC_CONCAT(test_, name),_init(void));                           \
    void CC_CONCAT(CC_CONCAT(test_, name),_update(void));                         \
    void CC_CONCAT(CC_CONCAT(test_, name),_draw(void));                           \
    void CC_CONCAT(CC_CONCAT(test_, name),_exit(void))

#define TEST_ENTRY_INITIALIZE(name) {                                          \
        CC_STRINGIFY(name),                                                    \
        CC_CONCAT(test_, CC_CONCAT(name, _init)),                              \
        CC_CONCAT(test_, CC_CONCAT(name, _update)),                            \
        CC_CONCAT(test_, CC_CONCAT(name, _draw)),                              \
        CC_CONCAT(test_, CC_CONCAT(name, _exit))                               \
    }

TEST_PROTOTYPE_DECLARE(normal_sprite_00);
TEST_PROTOTYPE_DECLARE(normal_sprite_01);
TEST_PROTOTYPE_DECLARE(normal_sprite_02);
TEST_PROTOTYPE_DECLARE(normal_sprite_03);
TEST_PROTOTYPE_DECLARE(normal_sprite_04);
TEST_PROTOTYPE_DECLARE(normal_sprite_05);

TEST_PROTOTYPE_DECLARE(scaled_sprite_00);

struct test tests_primitive[];
struct test tests_color[];
struct test tests_special_functions[];

struct test *tests[] = {
        tests_primitive,
        tests_color,
        tests_special_functions
};

struct test tests_primitive[] = {
        TEST_ENTRY_INITIALIZE(normal_sprite_00),
        /* ... */
        TEST_ENTRY_INITIALIZE(scaled_sprite_00)
};

struct test tests_color[] = {
};

struct test tests_special_functions[] = {
};
