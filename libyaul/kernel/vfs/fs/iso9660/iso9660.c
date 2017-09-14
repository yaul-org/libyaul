/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "iso9660-internal.h"

#include "iso9660.h"

typedef struct {
        iso9660_dirent_t dirent_root;
        uint16_t logical_block;
} iso9660_mnt_t;

extern int normalize_path(const char *, uint32_t x, uint32_t y,
    char ([y])[x]);

static const iso9660_dirent_t *iso9660_find(const iso9660_mnt_t *,
    const char *);
static const iso9660_dirent_t *iso9660_find_file(const iso9660_mnt_t *,
    const char *);
static const iso9660_dirent_t *iso9660_find_directory(const iso9660_mnt_t *,
    const char *);

void *
iso9660_open(void *p, const char *path, int mode __unused)
{
        iso9660_mnt_t *mnt;
        mnt = (iso9660_mnt_t *)p;

        const iso9660_dirent_t *dirent;
        dirent = iso9660_find_file(mnt, path);

        return (void *)dirent;
}

void
iso9660_close(void *fh __unused)
{
}

ssize_t
iso9660_read(void *p __unused, void *buf __unused, size_t bytes __unused)
{
        iso9660_mnt_t *mnt __unused;
        iso9660_dirent_t *xp __unused;

        mnt = (iso9660_mnt_t *)p;

        return 0;
}

void *
iso9660_mount(const char *mnt_point __unused)
{
        iso9660_mnt_t *mnt;
        mnt = NULL;

        /* Skip IP.BIN (16 sectors) */
        const struct iso_volume_descriptor *vd;
        vd = (const struct iso_volume_descriptor *)bread(16);

        /* Must be a "Primary Volume Descriptor" */
        if (isonum_711(vd->type) != ISO_VD_PRIMARY) {
                goto error;
        }
        /* Must match 'CD001' */
        if ((strncmp((const char *)vd->id, ISO_STANDARD_ID,
                    sizeof(vd->id) + 1)) == 0) {
                goto error;
        }

        /* We are interested in the Primary Volume Descriptor, which
         * points us to the root directory and path tables, which both
         * allow us to find any file on the CD */
        const struct iso_primary_descriptor *pd;
        pd = (const struct iso_primary_descriptor *)vd;

        if ((mnt = (iso9660_mnt_t *)malloc(sizeof(iso9660_mnt_t))) == NULL) {
                goto error;
        }

        /* Populate filesystem mount structure */
        /* Copy of directory record */
        (void)memcpy(&mnt->dirent_root, pd->root_directory_record,
            sizeof(mnt->dirent_root));

        /* Logical block size must be either 2048 or 2352 bytes */
        mnt->logical_block = isonum_723(pd->logical_block_size);
        if ((mnt->logical_block != 2048) && (mnt->logical_block != 2352)) {
                goto error;
        }

        return mnt;

error:
        if (mnt != NULL) {
                free(mnt);
        }

        return NULL;
}

static const iso9660_dirent_t *
iso9660_find_file(const iso9660_mnt_t *mnt, const char *path)
{
        const iso9660_dirent_t *dirent;
        dirent = iso9660_find(mnt, path);
        if (dirent == NULL) {
                /* XXX: errno = */
                return NULL;
        }

        uint8_t file_flags;
        file_flags = isonum_711(dirent->file_flags);

        uint8_t flags;
        flags = DIRENT_FILE_FLAGS_HIDDEN | DIRENT_FILE_FLAGS_DIRECTORY;
        if ((file_flags & flags) != DIRENT_FILE_FLAGS_FILE) {
                /* XXX: errno = */
                return NULL;
        }

        return dirent;
}

static const iso9660_dirent_t *
iso9660_find_directory(const iso9660_mnt_t *mnt, const char *path)
{
        const iso9660_dirent_t *dirent;
        dirent = iso9660_find(mnt, path);
        if (dirent == NULL) {
                /* XXX: errno = */
                return NULL;
        }

        uint8_t file_flags;
        file_flags = isonum_711(dirent->file_flags);

        uint8_t flags;
        flags = DIRENT_FILE_FLAGS_HIDDEN | DIRENT_FILE_FLAGS_DIRECTORY;
        if ((file_flags & flags) != DIRENT_FILE_FLAGS_DIRECTORY) {
                /* XXX: errno = */
                return NULL;
        }

        return dirent;
}

static const iso9660_dirent_t *
iso9660_find(const iso9660_mnt_t *mnt, const char *path)
{
        static char path_normalized[ISO_DIR_LEVEL_MAX][ISO_FILENAME_MAX_LENGTH];

        if ((normalize_path(path, ISO_FILENAME_MAX_LENGTH, ISO_DIR_LEVEL_MAX,
                    path_normalized)) < 0) {
                /* XXX: errno = */
                return NULL;
        }

        uint32_t sector;
        sector = isonum_733(mnt->dirent_root.extent);

        const iso9660_dirent_t *dirent;
        dirent = NULL;

        const iso9660_dirent_t *match_dirent;
        match_dirent = NULL;

        uint32_t component_idx;
        for (component_idx = 0; component_idx < ISO_DIR_LEVEL_MAX;
             component_idx++) {
                const char *component;
                component = &path_normalized[component_idx][0];
                size_t component_len;
                component_len = strlen(component);

                if (*component == '\0') {
                        return match_dirent;
                }

                uint32_t dirent_cnt;
                dirent_cnt = 0;

                bool match;
                match = false;

                do {
                        if ((dirent == NULL) || (isonum_711(dirent->length) == 0)) {
                                dirent = (const iso9660_dirent_t *)bread(sector);

                                if (dirent->name[0] == '\0') {
                                        dirent_cnt++;
                                }

                                /* Interleave mode must be disabled */
                                if ((isonum_711(dirent->file_unit_size) != 0) ||
                                    (isonum_711(dirent->interleave) != 0)) {
                                        /* XXX: errno = */
                                        return NULL;
                                }

                                /* If the third directory entry name found in
                                 * the current or immediate directory entry name
                                 * in the subsequent sector is '\0', then the
                                 * end of the current directory has been
                                 * reached. */
                                if (dirent_cnt == 3) {
                                        return NULL;
                                }

                                dirent_cnt = 0;

                                /* Next sector */
                                sector++;
                        }

                        /* Check for Current directory ('\0') or
                         * parent directory ('\1') */
                        if ((dirent->name[0] == '\0') || (dirent->name[0] == '\1')) {
                                dirent_cnt++;
                        } else if ((strncmp(component, dirent->name, component_len)) == 0) {
                                uint8_t file_flags;
                                file_flags = isonum_711(dirent->file_flags);

                                bool file;
                                file = (file_flags & DIRENT_FILE_FLAGS_HIDDEN) ==
                                    DIRENT_FILE_FLAGS_FILE;
                                bool directory;
                                directory = (file_flags & DIRENT_FILE_FLAGS_DIRECTORY) ==
                                    DIRENT_FILE_FLAGS_DIRECTORY;

                                if (file || directory) {
                                        match = true;
                                        match_dirent = dirent;
                                        sector = isonum_733(match_dirent->extent);
                                        dirent = NULL;
                                }
                        }

                        if (dirent != NULL) {
                                dirent = (const iso9660_dirent_t *)((uintptr_t)dirent +
                                    isonum_711(dirent->length));
                        }
                } while (!match);
        }

        return match_dirent;
}
