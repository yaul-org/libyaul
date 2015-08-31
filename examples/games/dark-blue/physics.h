/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include "aabb.h"
#include "collider.h"
#include "object.h"
#include "rigid_body.h"

void physics_init(void);
void physics_object_add(struct object *);
void physics_update(void);

#endif /* !PHYSICS_H */
