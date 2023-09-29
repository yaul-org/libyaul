/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_TLIST_H_
#define _MIC3D_TLIST_H_

#include "mic3d/types.h"

typedef struct tlist {
    const texture_t *textures;
    uint16_t texture_count;
} tlist_t;

void __tlist_init(void);

#endif /* _MIC3D_TLIST_H_ */
