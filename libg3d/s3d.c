#include <g3d/s3d.h>

#define PATCH_ADDRESS(s3d, x) ((void *)((uintptr_t)(x) + (uintptr_t)(s3d)))

static void _textures_patch(g3d_s3d_t *s3d, g3d_s3d_memory_usage_t *memory_usage);
static void _palettes_patch(g3d_s3d_t *s3d, g3d_s3d_memory_usage_t *memory_usage);
static void _texture_chain_patch(g3d_s3d_t *s3d);
static void _palette_chain_patch(g3d_s3d_t *s3d);
static void _object_patch(g3d_s3d_t *s3d, g3d_s3d_object_t *object);
static void _pictures_patch(g3d_s3d_t *s3d __unused, g3d_s3d_object_t *object);
static void _gouraud_tables_patch(g3d_s3d_t *s3d, g3d_s3d_object_t *object,
    g3d_s3d_memory_usage_t *memory_usage);

void
g3d_s3d_patch(g3d_s3d_t *s3d, g3d_s3d_memory_usage_t *memory_usage)
{
        g3d_s3d_object_t * const s3d_objects = g3d_s3d_objects_get(s3d);

        s3d->eof = PATCH_ADDRESS(s3d, s3d->eof);

        _texture_chain_patch(s3d);
        _palette_chain_patch(s3d);

        _textures_patch(s3d, memory_usage);
        _palettes_patch(s3d, memory_usage);

        for (uint32_t xpdata_index = 0; xpdata_index < s3d->object_count; xpdata_index++) {
                g3d_s3d_object_t * const object = &s3d_objects[xpdata_index];

                _object_patch(s3d, object);
                _pictures_patch(s3d, object);
                _gouraud_tables_patch(s3d, object, memory_usage);
        }
}

g3d_s3d_object_t *
g3d_s3d_objects_get(const g3d_s3d_t *s3d)
{
        return (void *)((uintptr_t)s3d + sizeof(g3d_s3d_t));
}

void *
g3d_s3d_texture_base_get(const g3d_s3d_texture_t *texture)
{
        return (void *)((uintptr_t)texture + sizeof(g3d_s3d_texture_t));
}

void *
g3d_s3d_palette_base_get(const g3d_s3d_palette_t *palette)
{
        return (void *)((uintptr_t)palette + sizeof(g3d_s3d_palette_t));
}

size_t
g3d_s3d_texture_size_get(const g3d_s3d_texture_t *texture)
{
        if (texture->eol) {
                return texture->size;
        }

        void * const texture_base = g3d_s3d_texture_base_get(texture);

        return ((uintptr_t)texture->next - (uintptr_t)texture_base);
}

size_t
g3d_s3d_palette_size_get(const g3d_s3d_palette_t *palette)
{
        if (palette->eol) {
                return palette->size;
        }

        void * const palette_base = g3d_s3d_palette_base_get(palette);

        return ((uintptr_t)palette->next - (uintptr_t)palette_base);
}

vdp1_gouraud_table_t *
g3d_s3d_object_gtb_base_get(const g3d_s3d_object_t *object)
{
        /* Get the gouraud table index from the first polygon and convert it to
         * a VDP1 VRAM address */
        const XPDATA * const xpdata = &object->xpdata;

        return (vdp1_gouraud_table_t *)VDP1_VRAM((uintptr_t)xpdata->attbl[0].gstb << 3);
}

static void
_textures_patch(g3d_s3d_t *s3d, g3d_s3d_memory_usage_t *memory_usage)
{
        if (s3d->textures_count == 0) {
                return;
        }

        s3d->textures = PATCH_ADDRESS(s3d, s3d->textures);

        g3d_s3d_texture_t * texture;
        texture = s3d->texture_datas;

        for (uint32_t i = 0; i < s3d->textures_count; i++) {
                TEXTURE * const texturesgl = &s3d->textures[i];

                const size_t texture_size = g3d_s3d_texture_size_get(texture);

                texturesgl->CGadr = (uintptr_t)memory_usage->texture >> 3;

                memory_usage->texture =
                    (void *)((uintptr_t)memory_usage->texture + texture_size);

                texture = texture->next;
        }
}

static void
_texture_chain_patch(g3d_s3d_t *s3d)
{
        if (s3d->texture_datas == NULL) {
                return;
        }

        s3d->texture_datas = PATCH_ADDRESS(s3d, s3d->texture_datas);

        g3d_s3d_texture_t * texture;
        texture = s3d->texture_datas;

        while (true) {
                if (texture->eol) {
                        break;
                }

                texture->next = PATCH_ADDRESS(s3d, texture->next);

                texture = texture->next;
        }
}

static void
_palettes_patch(g3d_s3d_t *s3d, g3d_s3d_memory_usage_t *memory_usage)
{
        if (s3d->palettes_count == 0) {
                return;
        }

        s3d->palettes = PATCH_ADDRESS(s3d, s3d->palettes);

        g3d_s3d_palette_t * palette;
        palette = s3d->palette_datas;

        for (uint32_t i = 0; i < s3d->palettes_count; i++) {
                PALETTE * const palettesgl = &s3d->palettes[i];

                /* For now, only VDP1 CLUTs (mode 1) will be supported */
                if (!palettesgl->Color) {
                        palettesgl->Color = (uintptr_t)memory_usage->clut >> 5;

                        memory_usage->clut++;
                } else {
                        const size_t palette_size = (g3d_s3d_palette_size_get(palette)) >> 2;

                        palettesgl->Color = (uintptr_t)memory_usage->cram >> 5;

                        memory_usage->cram += palette_size;
                }

                palette = palette->next;
        }
}

static void
_palette_chain_patch(g3d_s3d_t *s3d)
{
        if (s3d->palette_datas == NULL) {
                return;
        }

        s3d->palette_datas = PATCH_ADDRESS(s3d, s3d->palette_datas);

        g3d_s3d_palette_t * palette;
        palette = s3d->palette_datas;

        while (true) {
                if (palette->eol) {
                        break;
                }

                palette->next = PATCH_ADDRESS(s3d, palette->next);

                palette = palette->next;
        }
}

static void
_object_patch(g3d_s3d_t *s3d, g3d_s3d_object_t *object)
{
        object->xpdata.pntbl = PATCH_ADDRESS(s3d, object->xpdata.pntbl);
        object->xpdata.pltbl = PATCH_ADDRESS(s3d, object->xpdata.pltbl);
        object->xpdata.attbl = PATCH_ADDRESS(s3d, object->xpdata.attbl);
        object->xpdata.vntbl = PATCH_ADDRESS(s3d, object->xpdata.vntbl);

        object->pictures = PATCH_ADDRESS(s3d, object->pictures);
        object->gouraud_tables = PATCH_ADDRESS(s3d, object->gouraud_tables);
}

static void
_pictures_patch(g3d_s3d_t *s3d __unused, g3d_s3d_object_t *object)
{
        if (object->picture_count == 0) {
                return;
        }

        for (uint32_t i = 0; i < object->picture_count; i++) {
                PICTURE * const picture = &object->pictures[i];

                picture->pcsrc = PATCH_ADDRESS(s3d, picture->pcsrc);
        }
}

static void
_gouraud_tables_patch(g3d_s3d_t *s3d __unused, g3d_s3d_object_t *object,
    g3d_s3d_memory_usage_t *memory_usage)
{
        if (object->gouraud_table_count == 0) {
                return;
        }

        XPDATA * const xpdata = &object->xpdata;

        for (uint32_t i = 0; i < xpdata->nbPolygon; i++) {
                xpdata->attbl[i].gstb += (uintptr_t)memory_usage->gouraud_table >> 3;
        }

        memory_usage->gouraud_table += object->gouraud_table_count;
}
