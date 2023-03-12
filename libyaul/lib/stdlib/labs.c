/*
 * Copyright (c) 2012-2018 Romulo Fernandes
 * See LICENSE for details.
 *
 * Romulo Fernandes <abra185@gmail.com>
 */

#include <sys/cdefs.h>

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

long
labs(long number)
{
    return (number < 0) ? -number : number;
}
