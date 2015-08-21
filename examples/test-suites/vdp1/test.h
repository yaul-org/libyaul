/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef TEST_H
#define TEST_H

#include <fixmath.h>

#include "globals.h"

extern int32_t lut_cos[512];
extern int32_t lut_sin[512];

extern char *text;
extern size_t text_len;

void test_init(void);
void test_exit(void);

/* Functions for tests */
void sleep(fix16_t);

#endif /* !TEST_H */
