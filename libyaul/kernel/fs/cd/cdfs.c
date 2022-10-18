/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <cd-block.h>

#include <scu/map.h>

#include "cdfs-internal.h"

#include "cdfs.h"

static struct {
        cdfs_pvd_t pvd;
} _state;

static void _dirent_root_walk(cdfs_filelist_t *filelist,
    cdfs_filelist_walk_t walker, void *args);
static void _dirent_walk(cdfs_filelist_t *filelist,
    cdfs_filelist_walk_t walker, sector_t sector, void *args);
static bool _dirent_interleave(const cdfs_dirent_t *dirent);

static void _filelist_entry_populate(const cdfs_dirent_t *dirent,
    cdfs_entry_type_t type, cdfs_filelist_entry_t *filelist_entry);
static void _filelist_read_walker(cdfs_filelist_t *filelist,
        const cdfs_filelist_entry_t *entry, void *args);

static uint32_t _filelist_entry_count_clamp(int32_t count);

static uint8_t _sector_buffer[CDFS_SECTOR_SIZE] __aligned(CDFS_SECTOR_SIZE);

void
cdfs_filelist_default_init(cdfs_filelist_t *filelist,
    cdfs_filelist_entry_t *entries, int32_t count)
{
        cdfs_filelist_init(filelist, entries, cdfs_sector_read, count);
}

void
cdfs_filelist_init(cdfs_filelist_t *filelist,
    cdfs_filelist_entry_t *entries,
    cdfs_sector_read_t sector_read,
    int32_t count)
{
        assert(filelist != NULL);
        assert(entries != NULL);
        assert(sector_read != NULL);

        filelist->sector_read = sector_read;
        filelist->entries = entries;
        filelist->entries_pooled_count = _filelist_entry_count_clamp(count);
        filelist->entries_count = 0;
}

cdfs_filelist_entry_t *
cdfs_entries_alloc(int32_t count)
{
        const uint32_t clamped_count = _filelist_entry_count_clamp(count);

        return malloc(sizeof(cdfs_filelist_entry_t) * clamped_count);
}

void
cdfs_filelist_entries_free(cdfs_filelist_entry_t *entries)
{
        assert(entries != NULL);

        free(entries);
}

void
cdfs_filelist_root_read(cdfs_filelist_t *filelist)
{
        assert(filelist != NULL);

        cdfs_filelist_walk(filelist, NULL, _filelist_read_walker, NULL);
}

void
cdfs_filelist_read(cdfs_filelist_t *filelist,
    const cdfs_filelist_entry_t root_entry)
{
        assert(filelist != NULL);

        cdfs_filelist_walk(filelist, &root_entry, _filelist_read_walker, NULL);
}

void
cdfs_filelist_walk(cdfs_filelist_t *filelist,
    const cdfs_filelist_entry_t *root_entry,
    cdfs_filelist_walk_t walker,
    void *args)
{
        const cdfs_sector_read_t sector_read = filelist->sector_read;

        if (root_entry == NULL) {
                /* Skip IP.BIN (16 sectors) */
                sector_read(16, &_state.pvd);

                /* We are interested in the Primary Volume Descriptor, which
                 * points us to the root directory and path tables, which both
                 * allow us to find any file on the CD */

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

                /* Logical block size must be CDFS_SECTOR_SIZE bytes */
                assert(isonum_723(_state.pvd.logical_block_size) == CDFS_SECTOR_SIZE);

                _dirent_root_walk(filelist, walker, args);
        } else {
                const uint32_t sector = FAD2LBA(root_entry->starting_fad);

                _dirent_walk(filelist, walker, sector, args);
        }
}

static void
_filelist_read_walker(cdfs_filelist_t *filelist,
    const cdfs_filelist_entry_t *entry, void *args __unused)
{
        if (filelist->entries_count >= filelist->entries_pooled_count) {
                return;
        }

        cdfs_filelist_entry_t *this_entry;
        this_entry = &filelist->entries[filelist->entries_count];

        (void)memcpy(this_entry, entry, sizeof(cdfs_filelist_entry_t));

        filelist->entries_count++;
}

static bool __unused
_dirent_interleave(const cdfs_dirent_t *dirent)
{
        return ((isonum_711(dirent->interleave)) != 0x00);
}

static void
_dirent_root_walk(cdfs_filelist_t *filelist, cdfs_filelist_walk_t walker,
    void *args)
{
        cdfs_dirent_t dirent_root;

        /* Populate filesystem mount structure */
        /* Copy of directory record */
        (void)memcpy(&dirent_root, _state.pvd.root_directory_record, sizeof(dirent_root));

        /* Start walking the root directory */
        const uint32_t sector = isonum_733(dirent_root.extent);

        _dirent_walk(filelist, walker, sector, args);
}

static void
_dirent_walk(cdfs_filelist_t *filelist, cdfs_filelist_walk_t walker,
    sector_t sector, void *args)
{
        const cdfs_sector_read_t sector_read =
            filelist->sector_read;

        const cdfs_dirent_t *dirent;
        dirent = NULL;

        int32_t dirent_sectors;
        dirent_sectors = INT32_MAX;

        /* Keep track of where we are within the sector */
        uint32_t dirent_offset;
        dirent_offset = 0;

        while (true) {
                uint8_t dirent_length =
                    (dirent != NULL) ? isonum_711(dirent->length) : 0;

                if ((dirent == NULL) ||
                    (dirent_length == 0) ||
                    ((dirent_offset + dirent_length) >= CDFS_SECTOR_SIZE)) {
                        dirent_sectors--;

                        if (dirent_sectors == 0) {
                                break;
                        }

                        dirent_offset = 0;

                        sector_read(sector, _sector_buffer);

                        dirent = (const cdfs_dirent_t *)_sector_buffer;
                        dirent_length = isonum_711(dirent->length);

                        if (dirent->name[0] == '\0') {
                                uint32_t data_length;
                                data_length = isonum_733(dirent->data_length);

                                dirent_sectors = cdfs_sector_count_round(data_length);
                        }

                        /* Interleave mode must be disabled */
                        assert(!(_dirent_interleave(dirent)));

                        /* Next sector */
                        sector++;
                }

                /* Check for Current directory ('\0') or parent directory ('\1') */
                if ((dirent->name[0] != '\0') && (dirent->name[0] != '\1')) {
                        /* Interleave mode must be disabled */
                        assert(!(_dirent_interleave(dirent)));

                        if (walker != NULL) {
                                const uint8_t file_flags = isonum_711(dirent->file_flags);

                                cdfs_entry_type_t type;
                                type = CDFS_ENTRY_TYPE_FILE;

                                if ((file_flags & DIRENT_FILE_FLAGS_DIRECTORY) == DIRENT_FILE_FLAGS_DIRECTORY) {
                                        type = CDFS_ENTRY_TYPE_DIRECTORY;
                                }

                                cdfs_filelist_entry_t filelist_entry;

                                _filelist_entry_populate(dirent, type, &filelist_entry);

                                walker(filelist, &filelist_entry, args);
                        }
                }

                if (dirent != NULL) {
                        const uintptr_t p = (uintptr_t)dirent + dirent_length;

                        dirent = (const cdfs_dirent_t *)p;

                        dirent_offset += dirent_length;
                }
        }
}

static void
_filelist_entry_populate(const cdfs_dirent_t *dirent, cdfs_entry_type_t type,
    cdfs_filelist_entry_t *filelist_entry)
{
        filelist_entry->type = type;
        filelist_entry->size = isonum_733(dirent->data_length);
        filelist_entry->starting_fad = LBA2FAD(isonum_733(dirent->extent));
        filelist_entry->sector_count = cdfs_sector_count_round(filelist_entry->size);

        uint8_t name_len;
        name_len = isonum_711(dirent->file_id_len);

        if (type == CDFS_ENTRY_TYPE_FILE) {
                /* Minus the ';1' */
                name_len -= 2;

                /* Empty extension */
                if ((name_len > 0) && (dirent->name[name_len - 1] == '.')) {
                        name_len--;
                }
        }

        (void)memcpy(filelist_entry->name, dirent->name, name_len);
        filelist_entry->name[name_len] = '\0';
}

static uint32_t
_filelist_entry_count_clamp(int32_t count)
{
        if (count <= 0) {
                return CDFS_FILELIST_ENTRIES_COUNT;
        }

        if (count >= CDFS_FILELIST_ENTRIES_COUNT) {
                return CDFS_FILELIST_ENTRIES_COUNT;
        }

        return count;
}
