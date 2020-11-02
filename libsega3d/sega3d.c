/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <sys/cdefs.h>

#include "sega3d.h"
#include "sega3d-internal.h"

Uint16
sega3d_polycount_get(const PDATA *pdata)
{
        return pdata->nbPolygon;
}

void
sega3d_cmdt_prepare(const PDATA *pdata, vdp1_cmdt_list_t *cmdt_list, Uint16 offset)
{
        assert(pdata != NULL);
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);

        cmdt_list->count = pdata->nbPolygon;

        vdp1_cmdt_t *base_cmdts;
        base_cmdts = &cmdt_list->cmdts[offset];

        for (uint32_t i = 0; i < cmdt_list->count; i++) {
                vdp1_cmdt_t *cmdt;
                cmdt = &base_cmdts[i];

                const ATTR *attr;
                attr = &pdata->attbl[i];
                
                cmdt->cmd_ctrl = attr->dir; /* We care about (Dir) and (Comm) bits */
                cmdt->cmd_link = 0x0000;
                cmdt->cmd_pmod = attr->atrb;
                cmdt->cmd_colr = attr->colno;

                /* Even when there is not texture list, there is the default
                 * texture that zeroes out CMDSRCA and CMDSIZE */
                const TEXTURE *texture;
                texture = sega3d_tlist_tex_get(attr->texno);

#ifdef DEBUG
                /* If the texture number is zero, it could imply no texture.
                 * Even if the texture list is empty, it's considered valid */
                if (attr->texno != 0) {
                        assert(attr->texno < sega3d_tlist_count_get());
                }
#endif /* DEBUG */

                cmdt->cmd_srca = texture->CGadr;
                cmdt->cmd_size = texture->HVsize;
        }
}
