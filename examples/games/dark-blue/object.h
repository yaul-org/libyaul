/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef OBJECT_H
#define OBJECT_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include <inttypes.h>

#include "collider.h"
#include "transform.h"
#include "rigid_body.h"

#define OBJECT_DECLARATIONS                                                    \
    bool active;                                                               \
    uint32_t id;                                                               \
    struct transform transform;                                                \
    struct collider *colliders[2];                                             \
    struct rigid_body *rigid_body;                                             \
    void (*on_init)(struct object *);                                          \
    void (*on_update)(struct object *);                                        \
    void (*on_draw)(struct object *);                                          \
    void (*on_destroy)(struct object *);                                       \
    void (*on_collision)(struct object *, struct object *,                     \
        const struct collider_info *);                                         \
    void (*on_trigger)(struct object *, struct object *);

#define OBJECT_CALL_EVENT(x, name, args...) do {                               \
        if (((struct object *)(x))->CC_CONCAT(on_, name) != NULL) {            \
            ((struct object *)(x))->CC_CONCAT(on_, name)((struct object *)(x), \
            ##args);                                                           \
        }                                                                      \
} while (false)

struct object {
        OBJECT_DECLARATIONS
} __may_alias;

#endif /* !OBJECT_H */
