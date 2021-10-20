/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "debug.h"
#include "drivers.h"
#include "shared.h"
#include "crc.h"
#include "api.h"

/* Maximum filename length */
#define PATH_ENTRY_FILENAME_LENGTH      (32)
/* Maximum number of path entries */
#define PATH_ENTRY_COUNT                (4096)

#define FILESERVER_SECTOR_SIZE (2048)

static uint8_t _sector_buffer[FILESERVER_SECTOR_SIZE];

typedef struct path_entry path_entry_t;

struct path_entry {
        char filename[PATH_ENTRY_FILENAME_LENGTH];

        size_t sector_count;
        size_t size;
        uint8_t *buffer;

        time_t time;
        crc_t checksum;
};

static path_entry_t _path_entries[PATH_ENTRY_COUNT];
static char *_server_cwd;

typedef enum {
        READ_STATUS_OK,
        READ_STATUS_STAT_ERROR,
        READ_STATUS_FILETYPE_CHANGE,
        READ_STATUS_OPEN_ERROR,
        READ_STATUS_OOM,
        READ_STATUS_READ_ERROR
} read_status_t;

static const char *_read_status_strs[] = {
        "READ_STATUS_OK",
        "READ_STATUS_STAT_ERROR",
        "READ_STATUS_FILETYPE_CHANGE",
        "READ_STATUS_OPEN_ERROR",
        "READ_STATUS_OOM",
        "READ_STATUS_READ_ERROR"
};

static void _fileserver_cmd_file(const struct device_driver *);
static void _fileserver_cmd_byte_size(const struct device_driver *);
static void _fileserver_cmd_sector_count(const struct device_driver *);
static void _fileserver_cmd_quit(const struct device_driver *);

static int _validate_dirpath(const char *dirpath);

static void _build_path_entries(const char *dirpath);
static void _purge_path_entries(void);

static read_status_t _path_entry_file_read(path_entry_t *);
static void _path_entry_cache_purge(path_entry_t *);
static path_entry_t *_path_entry_find(const char *);

void
fileserver_init(const char *dirpath)
{
        verbose_printf("Starting file server: \"%s\"\n", dirpath);

        int ret;

        /* Check validity of directory path (OS specific) */
        if ((ret = _validate_dirpath(dirpath)) < 0) {
                /* Error */
                return;
        }

        /* XXX: Install necessary signal handlers */

        /* Build the path entries (OS specific) */
        _build_path_entries(dirpath);
}

void
fileserver_exec(const struct device_driver *device, uint8_t command)
{
        static uint8_t buffer[256];

        switch (command) {
        case API_CMD_FILE:
                _fileserver_cmd_file(device);
                break;
        case API_CMD_BYTE_SIZE:
                _fileserver_cmd_byte_size(device);
                break;
        case API_CMD_SECTOR_COUNT:
                _fileserver_cmd_sector_count(device);
                break;
        }
}

void
fileserver_quit(const struct device_driver *device)
{
        _fileserver_cmd_quit(device);
}

static void
_fileserver_cmd_file(const struct device_driver *device)
{
        /* Host receive - command       - 1B
         * Host receive - filename      - 32B
         * Host receive - sector offset - 4B
         * Host send    - sector        - 2048B
         * Host send    - checksum      - 1B */

        static char filename[PATH_ENTRY_FILENAME_LENGTH];

        int ret;

        if (!(api_variable_read(device, filename, PATH_ENTRY_FILENAME_LENGTH))) {
                /* Error */
                return;
        }

        verbose_printf("Requesting to read file \"%s\"\n", filename);

        const path_entry_t *path_entry;
        path_entry = _path_entry_find(filename);

        if (path_entry == NULL) {
                verbose_printf("Error: File not found\n");
                /* Error */
                return;
        }

        uint32_t sector_offset;

        if (!(api_long_read(device, &sector_offset))) {
                verbose_printf("Error: Timed out attempting to read sector offset\n");
                /* Error */
                return;
        }

        verbose_printf("Requested sector offset: %lu\n", sector_offset);

        if (sector_offset >= path_entry->sector_count) {
                /* Error */
                return;
        }

        const uint8_t *buffer_offset;
        buffer_offset = &path_entry->buffer[sector_offset * FILESERVER_SECTOR_SIZE];

        (void)memset(_sector_buffer, 0x00, FILESERVER_SECTOR_SIZE);
        (void)memcpy(_sector_buffer, buffer_offset, FILESERVER_SECTOR_SIZE);

        for (uint32_t try = 0; try < API_RETRIES_COUNT; try++) {
                verbose_printf("Sending buffer\n");

                if ((ret = device->send(_sector_buffer, FILESERVER_SECTOR_SIZE)) < 0) {
                        verbose_printf("Error: Timed out sending buffer\n");
                        /* Error */
                        return;
                }

                verbose_printf("Sent buffer\n");

                verbose_printf("Waiting for CRC\n");

                /* Read CRC for this sector */
                crc_t read_sector_crc;
                if (!(api_byte_read(device, &read_sector_crc))) {
                        verbose_printf("Error: Timed out attempting to read CRC byte\n");
                        /* Error */
                        return;
                }

                crc_t sector_crc;
                sector_crc = crc_calculate(_sector_buffer, FILESERVER_SECTOR_SIZE);

                verbose_printf("CRC match: 0x%02X <?> 0x%02X\n", read_sector_crc, sector_crc);

                if (sector_crc == read_sector_crc) {
                        device->send(&read_sector_crc, 1);
                        break;
                }

                verbose_printf("Error: CRC mismatch. Trying again\n");

                uint8_t ret_byte;
                ret_byte = API_RET_ERROR;

                device->send(&ret_byte, 1);

                usleep(API_USLEEP);
        }

        verbose_printf("Request complete\n");
}

static void
_fileserver_cmd_byte_size(const struct device_driver *device)
{
        /* Host receive - command        - 1B
         * Host receive - filename       - 32B
         * Host send    - file byte size - 4B */

        static char filename[PATH_ENTRY_FILENAME_LENGTH];

        int ret;

        if (!(api_variable_read(device, filename, PATH_ENTRY_FILENAME_LENGTH))) {
                /* Error */
                return;
        }

        verbose_printf("Requesting sector count for file \"%s\"\n", filename);

        const path_entry_t *path_entry;
        path_entry = _path_entry_find(filename);

        uint32_t byte_size;

        if (path_entry != NULL) {
                byte_size = TO_BE(path_entry->size);

                verbose_printf("Sending (LE: 0x%08X) (BE: 0x%08X)\n", path_entry->size, byte_size);
        } else {
                verbose_printf("Error: File not found\n");
                /* Error */
                byte_size = -1;
        }

        if ((ret = device->send(&byte_size, sizeof(byte_size))) < 0) {
                verbose_printf("Error: Timed out sending sector size\n");
                /* Error */
                return;
        }
}

static void
_fileserver_cmd_sector_count(const struct device_driver *device)
{
        /* Host receive - command      - 1B
         * Host receive - filename     - 32B
         * Host send    - sector count - 4B */

        static char filename[PATH_ENTRY_FILENAME_LENGTH];

        int ret;

        if (!(api_variable_read(device, filename, PATH_ENTRY_FILENAME_LENGTH))) {
                /* Error */
                return;
        }

        verbose_printf("Requesting sector count for file \"%s\"\n", filename);

        const path_entry_t *path_entry;
        path_entry = _path_entry_find(filename);

        uint32_t sector_count;

        if (path_entry != NULL) {
                sector_count = TO_BE(path_entry->sector_count);

                verbose_printf("Sending (LE: 0x%08X) (BE: 0x%08X)\n", path_entry->size, sector_count);
        } else {
                verbose_printf("Error: File not found\n");
                /* Error */
                sector_count = -1;
        }

        if ((ret = device->send(&sector_count, sizeof(sector_count))) < 0) {
                verbose_printf("Error: Timed out sending sector size\n");
                /* Error */
                return;
        }
}

static void
_fileserver_cmd_quit(const struct device_driver *device __unused)
{
        _purge_path_entries();
}

static int
_validate_dirpath(const char *dirpath __unused)
{
        return 0;
}

static read_status_t
_path_entry_file_read(path_entry_t *path_entry)
{
        struct stat statbuf;

        /* Attempt to get the modification time */
        int ret;
        if ((ret = stat(path_entry->filename, &statbuf)) < 0) {
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
        path_entry->sector_count =
            (size_t)(ceilf((float)path_entry->size / (float)FILESERVER_SECTOR_SIZE));
        path_entry->time = statbuf.st_mtime;

        read_status_t read_status = READ_STATUS_OK;

        FILE *fp;
        if ((fp = fopen(path_entry->filename, "rb")) == NULL) {
                read_status = READ_STATUS_OPEN_ERROR;
                goto error;
        }

        void *buffer;
        if ((buffer = malloc(path_entry->sector_count * FILESERVER_SECTOR_SIZE)) == NULL) {
                fclose(fp);

                read_status = READ_STATUS_OOM;
                goto error;
        }

        (void)memset(buffer, 0x00, path_entry->sector_count * FILESERVER_SECTOR_SIZE);

        path_entry->buffer = buffer;

        ssize_t size_read;
        size_read = fread(buffer, 1, statbuf.st_size, fp);

        fclose(fp);

        if (size_read != statbuf.st_size) {
                read_status = READ_STATUS_READ_ERROR;
                goto error;
        }

        crc_calculate(buffer, path_entry->sector_count * FILESERVER_SECTOR_SIZE);

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
        path_entry->sector_count = 0;
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

                if (*path_entry->filename == '\0') {
                        DEBUG_PRINTF("i: %i\n", i);
                        continue;
                }

                DEBUG_PRINTF("i: %i, filename: \"%s\" <?> \"%s\"\n", i, path_entry->filename, filename);

                if ((strstr(path_entry->filename, filename)) != NULL) {
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

        for (int file_count = 0; (dirent = readdir(dir)) != NULL; ) {
                struct stat statbuf;

                (void)sprintf(path, "%s/%s", dirpath, dirent->d_name);

                if ((strlen(dirent->d_name)) >= PATH_ENTRY_FILENAME_LENGTH) {
                        verbose_printf("Warning: Exceeded filename length: \"%s\"\n", dirent->d_name);

                        continue;
                }

                int ret;
                if ((ret = stat(path, &statbuf)) < 0) {
                        /* Error */
                        verbose_printf("Error: Stat error\n");

                        return;
                }

                /* Ignore anything but a regular file */
                if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
                        continue;
                }

                if (file_count >= PATH_ENTRY_COUNT) {
                        verbose_printf("Warning: Exceeded file count: %i/%i\n", file_count, PATH_ENTRY_COUNT);

                        continue;
                }

                path_entry_t *path_entry;
                path_entry = &_path_entries[file_count];

                path[PATH_ENTRY_FILENAME_LENGTH - 1] = '\0';
                (void)strcpy(path_entry->filename, path);
                path_entry->filename[PATH_ENTRY_FILENAME_LENGTH - 1] = '\0';

                _path_entry_cache_purge(path_entry);

                read_status_t read_status;
                read_status = _path_entry_file_read(path_entry);

                verbose_printf("Serving file: \"%s\", byte size: %lu, sector count: %lu\n",
                    path_entry->filename,
                    path_entry->size,
                    path_entry->sector_count);

                if (read_status != READ_STATUS_OK) {
                        verbose_printf("Error: Read status is invalid: %s\n", _read_status_strs[read_status]);

                        goto error;
                }

                file_count++;
        }

error:
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
