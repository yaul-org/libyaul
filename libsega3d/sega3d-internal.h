/*
 * No copyright.
 */

#ifndef SEGA3D_INTERNAL_H_
#define SEGA3D_INTERNAL_H_

#include <stdint.h>

#include "sega3d.h"

#include <vdp1/cmdt.h>

typedef enum {
        TLIST_FLAGS_NONE,
        TLIST_FLAGS_USER_ALLOCATED = 1 << 0,
        TLIST_FLAGS_ALLOCATED      = 1 << 1
} tlist_flags_t;

#endif /* SEGA3D_INTERNAL_H_ */
