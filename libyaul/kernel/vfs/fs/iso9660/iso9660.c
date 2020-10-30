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

static void _dirent_root_walk(iso9660_filelist_walk_t, void *);
static bool _dirent_interleave(const iso9660_dirent_t *);

static void _filelist_read_walker(const iso9660_filelist_entry_t *, void *);

#ifdef __linux__
#define DEBUG

#define LBA2FAD(x) (x)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define LWRAM(x) (0x20200000 + (x))
#define MEMORY_WRITE(n, x, a)                                                   \
\
if (x != LWRAM(0)) {                                                            \
            (void)printf("0x%08X -> 0x%08X\n", x, a);                           \
    }

static void
_bread(uint32_t sector, void *ptr)
{
        static uint8_t buffer[2048];

        assert(sizeof(buffer) == 2048);

        FILE *fp;
        fp = fopen("test4.iso", "rb");
        assert(fp != NULL);

        fseek(fp, sector * 2048, SEEK_SET);

        (void)memset(buffer, 0, sizeof(buffer));
        (void)fread(buffer, 1, sizeof(buffer), fp);

        fclose(fp);

        (void)memcpy(ptr, buffer, sizeof(buffer));
        (void)printf("sector: 0x%x = %i\n", sector, sector);
}

void
vdp_sync(void)
{
}

void
dbgio_flush(void)
{
}

int
main(void)
{
        iso9660_filelist_t filelist;
        iso9660_filelist_read(&filelist, -1);

        for (uint32_t i = 0; i < filelist.entries_count; i++) {
                (void)printf("\"%s\", %luB, %u\n", filelist.entries[i].name, filelist.entries[i].size, filelist.entries[i].sector_count);
        }

        return 0;
}
#else
static void
_bread(uint32_t sector, void *ptr)
{
        int ret;
        ret = cd_block_sector_read(LBA2FAD(sector), ptr);
        assert(ret == 0);
}
#endif /* __linux__ */

void
iso9660_filelist_read(iso9660_filelist_t *filelist, int32_t count)
{
        int32_t clamped_count;
        clamped_count = count;

        if (clamped_count <= 0) {
                clamped_count = ISO9660_FILELIST_ENTRIES_COUNT;
        } else if (clamped_count > ISO9660_FILELIST_ENTRIES_COUNT) {
                clamped_count = ISO9660_FILELIST_ENTRIES_COUNT;
        }

        filelist->entries = malloc(sizeof(iso9660_filelist_entry_t) * clamped_count);
        assert(filelist->entries != NULL);

        filelist->entries_pooled_count = (uint32_t)clamped_count;
        filelist->entries_count = 0;

        iso9660_filelist_walk(_filelist_read_walker, filelist);
}

void
iso9660_filelist_walk(iso9660_filelist_walk_t walker, void *args)
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
_filelist_read_walker(const iso9660_filelist_entry_t *entry, void *args)
{
        iso9660_filelist_t *filelist;
        filelist = args;

        if (filelist->entries_count >= filelist->entries_pooled_count) {
                return;
        }
        
        iso9660_filelist_entry_t *this_entry;
        this_entry = &filelist->entries[filelist->entries_count];

        (void)memcpy(this_entry, entry, sizeof(iso9660_filelist_entry_t));

        filelist->entries_count++;
}

static bool
_dirent_interleave(const iso9660_dirent_t *dirent)
{ 
        return ((isonum_711(dirent->interleave)) != 0x00);
}

static uint8_t _sector_buffer[2048] __aligned(0x800);

static void
_dirent_root_walk(iso9660_filelist_walk_t walker, void *args)
{

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

        /* Keep track of where we are within the sector */
        uint32_t dirent_offset;
        dirent_offset = 0;

        while (true) {
                uint8_t dirent_length;
                dirent_length = (dirent != NULL) ? isonum_711(dirent->length) : 0;

                if ((dirent == NULL) || (dirent_length == 0) || ((dirent_offset + dirent_length) >= 2048)) {
                        dirent_sectors--;

                        if (dirent_sectors == 0) {
                                break;
                        }

                        dirent_offset = 0;

                        _bread(sector, _sector_buffer);

                        dirent = (const iso9660_dirent_t *)&_sector_buffer[0];

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
                                assert(!(_dirent_interleave(dirent)));

                                if (walker != NULL) {
                                        iso9660_filelist_entry_t filelist_entry;

                                        filelist_entry.size = isonum_733(dirent->data_length);
                                        filelist_entry.starting_fad = LBA2FAD(isonum_733(dirent->extent));
                                        filelist_entry.sector_count = _length_sector_round(filelist_entry.size);

                                        uint8_t name_len;
                                        name_len = isonum_711(dirent->file_id_len);

                                        /* Minus the ';1' */
                                        (void)memset(filelist_entry.name, '\0', sizeof(filelist_entry.name));
                                        (void)memcpy(filelist_entry.name, dirent->name, name_len - 2);

                                        walker(&filelist_entry, args);
                                }
                        }
                }

                if (dirent != NULL) {
                        uintptr_t p;
                        p = (uintptr_t)dirent + dirent_length;

                        dirent = (const iso9660_dirent_t *)p;

                        dirent_offset += dirent_length;
                }
        }
}
