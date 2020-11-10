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

#define MATRIX_STACK_MAX        (20)
#define Z_RANGE                 (1024)
#define PACKET_SIZE             (4096)
#define VERTEX_POOL_SIZE        (1024)

typedef struct sort_single {
        void *packet;
        struct sort_single *next_single;
} sort_single_t;

typedef struct {
        sort_single_t *first_single;
} sort_list_t;

typedef void (*iterate_fn)(sort_single_t *);

#endif /* SEGA3D_INTERNAL_H_ */
