/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef TEST_H
#define TEST_H

void test_nop(void);
void test_init(void);
int test_register(const char *, void (*)(void), void (*)(void), void (*)(void), void (*)(void));
int test_load(const char *);

extern void (*test_update)(void);
extern void (*test_draw)(void);
extern void (*test_exit)(void);

#endif /* !TEST_H */
