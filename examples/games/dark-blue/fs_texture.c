/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include <assert.h>

#include "fs.h"

void
fs_texture_vdp1_load(const char *path, void *image_dst, void *cmap_dst)
{
        void *file_handle;
        file_handle = fs_open(path);

        size_t file_size;
        file_size = romdisk_total(file_handle);

        uint8_t *ptr;
        ptr = (uint8_t *)0x00201000;

        fs_read(file_handle, ptr, file_size);
        fs_close(file_handle);

        tga_t tga;
        int status;
        status = tga_read(&tga, ptr);
        assert(status == TGA_FILE_OK);
        uint32_t amount;
        amount = tga_image_decode(&tga, image_dst);
        assert(amount > 0);
        amount = tga_cmap_decode(&tga, cmap_dst);
        assert(amount > 0);
}

void
fs_texture_vdp2_load(const char *path, void *image_dst, void *cmap_dst)
{
        void *file_handle;
        file_handle = fs_open(path);

        size_t file_size;
        file_size = romdisk_total(file_handle);

        uint8_t *ptr;
        ptr = (uint8_t *)0x00201000;

        fs_read(file_handle, ptr, file_size);
        fs_close(file_handle);

        tga_t tga;
        int status;
        status = tga_read(&tga, ptr);
        assert(status == TGA_FILE_OK);
        uint32_t amount;
        amount = tga_image_decode_tiled(&tga, image_dst);
        assert(amount > 0);
        amount = tga_cmap_decode(&tga, cmap_dst);
        assert(amount > 0);
}
