/*
 * Copyright (c) 2012-2018 Romulo Fernandes
 * See LICENSE for details.
 *
 * Romulo Fernandes <abra185@gmail.com>
 */

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <internal.h>

int abs(int32_t number)
{
        return (number < 0) ? -number : number;
}
