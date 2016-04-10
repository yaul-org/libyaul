/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

#define OBJECT(x, member)                                                      \
        ((x)->CC_CONCAT(,member))

#define OBJECT_PUBLIC_DATA(x, member)                                          \
        ((x)->data.CC_CONCAT(m_,member))

#define OBJECT_PRIVATE_DATA(x, member)                                         \
        ((x)->private_data.CC_CONCAT(m_,member))

#define OBJECT_CALL_PUBLIC_MEMBER(x, name, args...)                            \
        ((x))->functions.CC_CONCAT(m_, name)((struct object *)(x), ##args)

#define THIS(type, member)                                                     \
        (((struct type *)this)->CC_CONCAT(,member))

#define THIS_PUBLIC_DATA(type, member)                                         \
        (((struct type *)this)->data.CC_CONCAT(m_, member))

#define THIS_PRIVATE_DATA(type, member)                                        \
        (((struct type *)this)->private_data.CC_CONCAT(m_, member))

#define THIS_CALL_PUBLIC_MEMBER(type, name, args...)                           \
        ((struct type *)this)->functions.CC_CONCAT(m_, name)(this, ##args)

#define THIS_CALL_PRIVATE_MEMBER(type, name, args...)                          \
        ((struct type *)this)->private_functions.CC_CONCAT(m_, name)(this, ##args)

struct object {
        OBJECT_DECLARATIONS
} __may_alias;

#endif /* !OBJECT_H */
