/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <ctype.h>
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "debug.h"
#include "drivers.h"
#include "shared.h"
#include "crc.h"

/* Maximum length of a path per path entry */
#define PATH_ENTRY_PATH_LENGTH  (256)
/* Maximum number of path entries */
#define PATH_ENTRY_COUNT        (4096)

typedef struct path_entry path_entry_t;

struct path_entry {
        char path[PATH_ENTRY_PATH_LENGTH];

        size_t size;
        void *buffer;

        time_t time;
        crc_t checksum;
};

static path_entry_t _path_entries[PATH_ENTRY_COUNT];

typedef enum {
        READ_STATUS_OK,
        READ_STATUS_STAT_ERROR,
        READ_STATUS_FILETYPE_CHANGE,
        READ_STATUS_OPEN_ERROR,
        READ_STATUS_OOM,
        READ_STATUS_READ_ERROR
} read_status_t;

static int _validate_dirpath(const char *dirpath);

static void _build_path_entries(const char *dirpath);
static void _purge_path_entries(void);

static read_status_t _path_entry_file_read(path_entry_t *);
static void _path_entry_cache_purge(path_entry_t *);

void
fileserver(const struct device_driver *device, const char *dirpath)
{
        verbose_printf("Starting file server: \"%s\"\n", dirpath);

        /* Check validity of directory path (OS specific) */
        int ret;
        if ((ret = _validate_dirpath(dirpath)) < 0) {
                /* Error */
                return;
        }

        /* Install necessary signal handlers */

        /* Build the path entries (OS specific) */
        _build_path_entries(dirpath);

        /* Start serving */
        while (true) {
                verbose_printf("Waiting...\n");
                break;
        }

        _purge_path_entries();
}

static int
_validate_dirpath(const char *dirpath)
{
        return 0;
}

static read_status_t
_path_entry_file_read(path_entry_t *path_entry)
{
        struct stat statbuf;

        /* Attempt to get the modification time */
        int ret;
        if ((ret = stat(path_entry->path, &statbuf)) < 0) {
                return READ_STATUS_STAT_ERROR;
        }

        if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
                return READ_STATUS_FILETYPE_CHANGE;
        }

        if ((path_entry->time == statbuf.st_mtime) &&
            (path_entry->buffer != NULL)) {
                /* No error - we're up to date */
                return  READ_STATUS_OK;
        }

        path_entry->size = statbuf.st_size;
        path_entry->time = statbuf.st_mtime;

        read_status_t read_status = READ_STATUS_OK;

        FILE *fp;
        if ((fp = fopen(path_entry->path, "rb")) == NULL) {
                read_status = READ_STATUS_OPEN_ERROR;
                goto error;
        }

        void *buffer;
        if ((buffer = malloc(path_entry->size)) == NULL) {
                fclose(fp);

                read_status = READ_STATUS_OOM;
                goto error;
        }

        path_entry->buffer = buffer;

        size_t size_read;
        size_read = fread(buffer, 1, path_entry->size, fp);

        fclose(fp);

        if (size_read != path_entry->size) {
                read_status = READ_STATUS_READ_ERROR;
                goto error;
        }

        crc_calculate(buffer, path_entry->size);

        return read_status;

error:
        _path_entry_cache_purge(path_entry);

        return read_status;
}

static void
_path_entry_cache_purge(path_entry_t *path_entry)
{
        if (path_entry->buffer != NULL) {
                free(path_entry->buffer);
        }

        path_entry->size = 0;
        path_entry->checksum = 0;

        path_entry->buffer = NULL;
        path_entry->time = 0;
}

static path_entry_t *
_path_entry_find(const char *filename)
{
        for (int i = 0; i < PATH_ENTRY_COUNT; i++) {
                path_entry_t *path_entry;
                path_entry = &_path_entries[i];

                if (*path_entry->path == '\0') {
                        return NULL;
                }

                if ((strstr(path_entry->path, filename)) != NULL) {
                        return path_entry;
                }
        }

        return NULL;
}

static void
_build_path_entries(const char *dirpath)
{
        static char path[2048];

        (void)memset(_path_entries, 0, sizeof(path_entry_t) * PATH_ENTRY_COUNT);

        DIR *dir;
        if ((dir = opendir(dirpath)) == NULL) {
                /* Error */
                return;
        }

        struct dirent *dirent;

        for (int i = 0; (dirent = readdir(dir)) != NULL; i++) {
                struct stat statbuf;

                (void)sprintf(path, "%s/%s", dirpath, dirent->d_name); 

                int ret;
                if ((ret = stat(path, &statbuf)) < 0) {
                        /* Error */
                        return;
                }

                if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
                        continue;
                }

                if (i >= PATH_ENTRY_COUNT) {
                        /* Warning/note */
                        continue;
                }

                path_entry_t *path_entry;
                path_entry = &_path_entries[i];

                (void)strncpy(path_entry->path, path, PATH_ENTRY_PATH_LENGTH);
                path_entry->path[PATH_ENTRY_PATH_LENGTH - 1] = '\0';

                _path_entry_cache_purge(path_entry);

                read_status_t read_status;
                read_status = _path_entry_file_read(path_entry);

                if (read_status != READ_STATUS_OK) {
                        /* Error */
                        return;
                }
        }

        closedir(dir);
}

static void
_purge_path_entries(void)
{
        for (int i = 0; i < PATH_ENTRY_COUNT; i++) {
                path_entry_t *path_entry;
                path_entry = &_path_entries[i];

                _path_entry_cache_purge(path_entry);
        }
}
