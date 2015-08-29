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

#define TEST_PROTOTYPE_DECLARE(name)                                           \
    void CC_CONCAT(CC_CONCAT(test_, name),_init(void));                        \
    void CC_CONCAT(CC_CONCAT(test_, name),_update(void));                      \
    void CC_CONCAT(CC_CONCAT(test_, name),_draw(void));                        \
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

TEST_PROTOTYPE_DECLARE(scaled_sprite_00);
TEST_PROTOTYPE_DECLARE(scaled_sprite_01);

static const struct test tests_primitive_normal_sprite[] = {
        TEST_ENTRY_INITIALIZE(normal_sprite_00),
};

static const struct test tests_primitive_scaled_sprite[] = {
        TEST_ENTRY_INITIALIZE(scaled_sprite_00),
        TEST_ENTRY_INITIALIZE(scaled_sprite_01)
};

static const struct test tests_primitive_distorted_sprite[] = {
};

static const struct test *tests_primitive[] = {
        tests_primitive_normal_sprite,
        tests_primitive_scaled_sprite,
        tests_primitive_distorted_sprite
};

static const struct test *tests_color[] = {
};

static const struct test *tests_special_functions[] = {
};

static const struct test **tests[] = {
        tests_primitive,
        tests_color,
        tests_special_functions
};

const struct test *
tests_fetch(uint32_t type, uint32_t subtype __unused, uint32_t idx __unused)
{
        const struct test **test_type;
        test_type = tests[type];

        const struct test *test_subtype;
        test_subtype = test_type[subtype];

        const struct test *test;
        test = &test_subtype[idx];

        return test;
}
