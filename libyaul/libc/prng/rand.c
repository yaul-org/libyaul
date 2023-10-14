/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>
#include <stdint.h>

#include <prng.h>

void __weak
srand(unsigned s)
{
    prng_xorshift32_seed(s);
}

int __weak
rand(void)
{
    return prng_xorshift32();
}
