#ifndef _SEGA3D_S3D_H_
#define _SEGA3D_S3D_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>

#include <vdp1.h>
#include <vdp2.h>

#include <sega3d/sgl.h>

typedef union sega3d_s3d_texture {
        struct {
                /* Flag to denote end of list */
                unsigned int eol:1;
                unsigned int size:31;
        } __packed;

        union sega3d_s3d_texture *next;
} __packed sega3d_s3d_texture_t;

static_assert(sizeof(sega3d_s3d_texture_t) == 4);

typedef union sega3d_s3d_palette {
        struct {
                /* Flag to denote end of list */
                unsigned int eol:1;
                unsigned int size:31;
        } __packed;

        union sega3d_s3d_palette *next;
} __packed sega3d_s3d_palette_t;

static_assert(sizeof(sega3d_s3d_palette_t) == 4);

typedef struct {
        char sig[4];
        uint32_t version;
        uint32_t flags;
        uint32_t object_count;

        TEXTURE *textures;
        uint32_t textures_count;

        PALETTE *palettes;
        uint32_t palettes_count;

        sega3d_s3d_texture_t *texture_datas;
        sega3d_s3d_palette_t *palette_datas;

        void *eof;
} __packed sega3d_s3d_t;

static_assert(sizeof(sega3d_s3d_t) == 44);

typedef struct {
        XPDATA xpdata;

        PICTURE *pictures;
        uint32_t picture_count;

        vdp1_gouraud_table_t *gouraud_tables;
        uint32_t gouraud_table_count;
} __packed sega3d_s3d_object_t;

static_assert(sizeof(sega3d_s3d_object_t) == 40);

typedef struct {
        void *texture;
        vdp1_clut_t *clut;
        vdp1_gouraud_table_t *gouraud_table;

        vdp2_cram_t *cram;
} sega3d_s3d_memory_usage_t;

static_assert(sizeof(sega3d_s3d_memory_usage_t) == 16);

extern void sega3d_s3d_patch(sega3d_s3d_t *s3d, sega3d_s3d_memory_usage_t *memory_usage);

extern sega3d_s3d_object_t *sega3d_s3d_objects_get(const sega3d_s3d_t *s3d);

extern void *sega3d_s3d_texture_base_get(const sega3d_s3d_texture_t *texture);
extern size_t sega3d_s3d_texture_size_get(const sega3d_s3d_texture_t *texture);

extern void *sega3d_s3d_palette_base_get(const sega3d_s3d_palette_t *palette);
extern size_t sega3d_s3d_palette_size_get(const sega3d_s3d_palette_t *palette);

extern vdp1_gouraud_table_t *sega3d_s3d_object_gtb_base_get(const sega3d_s3d_object_t *object);

#endif /* !_SEGA3D_S3D_H_ */
