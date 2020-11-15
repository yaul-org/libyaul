/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <sys/cdefs.h>

#include <vdp.h>
#include <vdp2/tvmd.h>

#include <cpu/cache.h>
#include <cpu/divu.h>
#include <cpu/frt.h>

#include "sega3d.h"
#include "sega3d-internal.h"

extern void _internal_fog_init(void);
extern void _internal_matrix_init(void);
extern void _internal_sort_init(void);
extern void _internal_tlist_init(void);
extern void _internal_transform_init(void);

void
sega3d_init(void)
{        
        /* Prevent re-initialization */
        if ((_internal_state.flags & FLAGS_INITIALIZED) != FLAGS_NONE) {
                return;
        }

        _internal_state.flags = FLAGS_INITIALIZED;

        sega3d_perspective_set(DEGtoANG(90.0f));

        _internal_tlist_init();
        _internal_matrix_init();
        _internal_fog_init();
        _internal_sort_init();
        _internal_transform_init();
}

void
sega3d_perspective_set(ANGLE fov)
{
#define AW_2 FIX16(12.446f) /* ([film-aperature-width = 0.980] * inch->mm) / 2) */
#define AH_2 FIX16(9.3345f) /* ([film-aperature-height = 0.735] * inch->mm) / 2) */

        transform_t * const trans = _internal_state.transform;

        const FIXED fov_angle = fix16_mul(fov, FIX16_2PI) >> 1;

        uint16_t i_width;
        uint16_t i_height;

        vdp2_tvmd_display_res_get(&i_width, &i_height);

        /* Let's maintain a 4:3 ratio */
        if (i_height < 240) {
                i_height = 240;
        }

        const FIXED screen_width = fix16_int32_from(i_width);
        const FIXED screen_height = fix16_int32_from(i_height);

        trans->cached_sw_2 = i_width / 2;
        trans->cached_sh_2 = i_height / 2;

        

        cpu_divu_fix16_set(screen_width, screen_height);
        _internal_state.info->ratio = cpu_divu_quotient_get();

        cpu_divu_fix16_set(FIX16(1.0f), fix16_tan(fov_angle));
        _internal_state.info->focal_length = fix16_mul(AW_2, cpu_divu_quotient_get());

        _internal_state.info->near = _internal_state.info->focal_length;

        cpu_divu_fix16_set(AH_2, _internal_state.info->focal_length);

        const FIXED top = cpu_divu_quotient_get();
        const FIXED right = -fix16_mul(-top, _internal_state.info->ratio);

        cpu_divu_fix16_set(screen_width, right << 1);
        trans->cached_inv_right = cpu_divu_quotient_get();
}

void
sega3d_info_get(sega3d_info_t *info)
{
        (void)memcpy(info, _internal_state.info, sizeof(sega3d_info_t));
}

Uint16
sega3d_object_polycount_get(const sega3d_object_t *object)
{
        const PDATA * const pdata = object->pdata;

        return pdata->nbPolygon;
}
