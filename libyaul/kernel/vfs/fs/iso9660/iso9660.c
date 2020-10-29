/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifndef __linux__
#include <cd-block.h>

#include <scu/map.h>
#endif /* __linux__ */

#include "iso9660-internal.h"

#include "iso9660.h"

static struct {
        iso9660_pvd_t pvd;
} _state;

static inline uint32_t __always_inline
_length_sector_round(uint32_t length)
{
        return ((length + 0x07FF) >> 11);
}

static void _dirent_root_walk(iso9660_toc_walk_t, void *);
static bool _dirent_interleave(const iso9660_dirent_t *);

static void _toc_read_walker(const iso9660_toc_entry_t *, void *);

#ifdef __linux__
#define DEBUG
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void
_bread(uint32_t sector, void *ptr)
{
        static uint8_t buffer[2048];

        FILE *fp;
        fp = fopen("test4.iso", "rb");
        assert(fp != NULL);

        fseek(fp, sector * 2048, SEEK_SET);

        (void)memset(buffer, 0, sizeof(buffer));
        (void)fread(buffer, 1, sizeof(buffer), fp);

        fclose(fp);

        (void)memcpy(ptr, buffer, sizeof(buffer));
        (void)printf("sector: $%x\n", sector);
}

int
main(void)
{
        iso9660_toc_t toc;
        iso9660_toc_read(&toc, -1);

        for (uint32_t i = 0; i < toc.entries_count; i++) {
                (void)printf("\"%s\", %luB, %u\n", toc.entries[i].name, toc.entries[i].size, toc.entries[i].sector_count);
        }

        return 0;
}
#else
static volatile uint8_t *_lwram __used = (volatile uint8_t *)LWRAM(0);

static void
_bread(uint32_t sector, void *ptr)
{
        cd_block_read_data(LBA2FAD(sector), 2048, ptr);

        /* (void)memcpy((void *)LWRAM(0), ptr, 2048); */
}
#endif /* __linux__ */

void
iso9660_toc_read(iso9660_toc_t *toc, int32_t count)
{
        int32_t clamped_count;
        clamped_count = count;

        if (clamped_count <= 0) {
                clamped_count = ISO9660_TOC_ENTRIES_COUNT;
        } else if (clamped_count > ISO9660_TOC_ENTRIES_COUNT) {
                clamped_count = ISO9660_TOC_ENTRIES_COUNT;
        }

        toc->entries = malloc(sizeof(iso9660_toc_entry_t) * clamped_count);
        assert(toc->entries != NULL);

        toc->entries_pooled_count = (uint32_t)clamped_count;
        toc->entries_count = 0;

        iso9660_toc_walk(_toc_read_walker, toc);
}

void
iso9660_toc_walk(iso9660_toc_walk_t walker, void *args)
{
        /* Skip IP.BIN (16 sectors) */
        _bread(16, &_state.pvd);

        /* We are interested in the Primary Volume Descriptor, which points us
         * to the root directory and path tables, which both allow us to find
         * any file on the CD */

        /* Must be a "Primary Volume Descriptor" */
        assert(isonum_711(_state.pvd.type) == ISO_VD_PRIMARY);

        /* Must match 'CD001' */
#ifdef DEBUG
        const char *cd001_str;
        cd001_str = (const char *)_state.pvd.id;
        size_t cd001_len;
        cd001_len = sizeof(_state.pvd.id) + 1;

        assert((strncmp(cd001_str, ISO_STANDARD_ID, cd001_len)) != 0);
#endif /* DEBUG */

        /* Logical block size must be 2048 bytes */
        assert(isonum_723(_state.pvd.logical_block_size) == 2048);

        _dirent_root_walk(walker, args);
}

static void
_toc_read_walker(const iso9660_toc_entry_t *entry, void *args)
{
        iso9660_toc_t *toc;
        toc = args;

        if (toc->entries_count >= toc->entries_pooled_count) {
                return;
        }
        
        iso9660_toc_entry_t *this_entry;
        this_entry = &toc->entries[toc->entries_count];

        (void)memcpy(this_entry, entry, sizeof(iso9660_toc_entry_t));

        toc->entries_count++;
}

static bool
_dirent_interleave(const iso9660_dirent_t *dirent)
{ 
        return ((isonum_711(dirent->interleave)) != 0x00);
}

static void
_dirent_root_walk(iso9660_toc_walk_t walker, void *args)
{
        static uint8_t sector_buffer[2048] __aligned(4);

        iso9660_dirent_t dirent_root;

        /* Populate filesystem mount structure */
        /* Copy of directory record */
        (void)memcpy(&dirent_root, _state.pvd.root_directory_record, sizeof(dirent_root));

        /* Start walking the root directory */
        uint32_t sector;
        sector = isonum_733(dirent_root.extent);

        const iso9660_dirent_t *dirent;
        dirent = NULL;

        int32_t dirent_sectors;
        dirent_sectors = INT32_MAX;

        while (true) {
                if ((dirent == NULL) || (isonum_711(dirent->length) == 0)) {
                        dirent_sectors--;

                        if (dirent_sectors == 0) {
                                break;
                        }

                        _bread(sector, sector_buffer);

                        dirent = (const iso9660_dirent_t *)&sector_buffer[0];

                        if (dirent->name[0] == '\0') {
                                uint32_t data_length;
                                data_length = isonum_733(dirent->data_length);

                                dirent_sectors = _length_sector_round(data_length);
                        }

                        /* Interleave mode must be disabled */
                        assert(!(_dirent_interleave(dirent)));

                        /* Next sector */
                        sector++;
                }

                /* Check for Current directory ('\0') or parent directory ('\1') */
                if ((dirent->name[0] != '\0') && (dirent->name[0] != '\1')) {
                        uint8_t file_flags;
                        file_flags = isonum_711(dirent->file_flags);

                        if ((file_flags & DIRENT_FILE_FLAGS_FILE) == DIRENT_FILE_FLAGS_FILE) {
                                /* Interleave mode must be disabled */
                                if (!(_dirent_interleave(dirent))) {
                                        iso9660_toc_entry_t toc_entry;

                                        toc_entry.size = isonum_733(dirent->data_length);
                                        toc_entry.starting_fad = isonum_733(dirent->extent);
                                        toc_entry.sector_count = _length_sector_round(toc_entry.size);

                                        uint8_t name_len;
                                        name_len = isonum_711(dirent->file_id_len);

                                        /* Minus the ';1' */
                                        (void)memset(toc_entry.name, '\0', sizeof(toc_entry.name));
                                        (void)memcpy(toc_entry.name, dirent->name, name_len - 2);

                                        if (walker != NULL) {
                                                walker(&toc_entry, args);
                                        }
                                }
                        }
                }

                if (dirent != NULL) {
                        uintptr_t p;
                        p = (uintptr_t)dirent + isonum_711(dirent->length);

                        dirent = (const iso9660_dirent_t *)p;
                }
        }
}
