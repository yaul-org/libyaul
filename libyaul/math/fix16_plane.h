/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _FIX16_INCLUDE_ONCE
#error "Header file must not be directly included"
#endif /* !_FIX16_INCLUDE_ONCE */

#define FIX16_PLANE_INITIALIZER(nx, ny, nz, dx, dy, dz)                        \
    {                                                                          \
            .normal = {                                                        \
                    FIX16(nx),                                                 \
                    FIX16(ny),                                                 \
                    FIX16(nz)                                                  \
            },                                                                 \
            .d = {                                                             \
                    FIX16(dx),                                                 \
                    FIX16(dy),                                                 \
                    FIX16(dz)                                                  \
            }                                                                  \
    }

typedef struct fix16_plane {
        fix16_vec3_t normal;
        fix16_vec3_t d;
} __packed __aligned(4) fix16_plane_t;

extern uint32_t fix16_plane_str(const fix16_plane_t *, char *, int);
