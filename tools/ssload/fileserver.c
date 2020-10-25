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

/* Maximum length of a path per path entry */
#define PATH_ENTRY_PATH_LENGTH  (256)
/* Maximum number of path entries */
#define PATH_ENTRY_COUNT        (4096)

#define FILESERVER_SLEEP        (16666)

#define FILESERVER_SECTOR_SIZE  (2048)

#define FILESERVER_CMD_FILE     (0x00)
#define FILESERVER_CMD_SIZE     (0x01)
#define FILESERVER_CMD_QUIT     (0x7F)
#define FILESERVER_CMD_INVALID  (0xFF)

#define FILESERVER_RET_ERROR    (0xFF)

static uint8_t _sector_buffer[FILESERVER_SECTOR_SIZE];

typedef struct path_entry path_entry_t;

struct path_entry {
        char path[PATH_ENTRY_PATH_LENGTH];

        size_t size;
        uint8_t *buffer;

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

static void _fileserver_cmd_file(const struct device_driver *);
static void _fileserver_cmd_size(const struct device_driver *);
static void _fileserver_cmd_quit(const struct device_driver *);

static int _validate_dirpath(const char *dirpath);

static void _build_path_entries(const char *dirpath);
static void _purge_path_entries(void);

static read_status_t _path_entry_file_read(path_entry_t *);
static void _path_entry_cache_purge(path_entry_t *);
static path_entry_t *_path_entry_find(const char *);

void
fileserver(const struct device_driver *device, const char *dirpath)
{
        static uint8_t buffer[256];

        verbose_printf("Starting file server: \"%s\"\n", dirpath);

        int ret;

        /* Check validity of directory path (OS specific) */
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

                if ((ret = device->read(buffer, 1)) < 0) {
                        sleep(FILESERVER_SLEEP);
                        continue;
                }

                DEBUG_PRINTF("Command: 0x%02X\n", buffer[0]);

                switch (buffer[0]) {
                case FILESERVER_CMD_FILE:
                        _fileserver_cmd_file(device);
                        break;
                case FILESERVER_CMD_SIZE:
                        _fileserver_cmd_size(device);
                        break;
                case FILESERVER_CMD_QUIT:
                        _fileserver_cmd_quit(device);
                        goto shutdown;
                }
        }

shutdown:
        ;
}

#define FILESERVER_TRIES 1000

static bool
_variable_read(const struct device_driver *device, uint8_t *buffer, uint32_t buffer_len)
{
        (void)memset(buffer, 0x00, buffer_len);

        for (uint32_t i = 0; i < FILESERVER_TRIES; i++) {
                int ret;
                if ((ret = device->read(buffer, buffer_len)) >= 0) {
                        return true;
                }

                sleep(FILESERVER_SLEEP);
        }

        return false;
}

static bool
_long_read(const struct device_driver *device, uint32_t *value)
{
        *value = 0;

        bool read;
        read = _variable_read(device, value, sizeof(uint32_t));

        *value = TOLSB(*value);

        return read;
}

static bool
_byte_read(const struct device_driver *device, uint8_t *value)
{
        *value = 0;

        return _variable_read(device, value, sizeof(uint8_t));
}

static void
_fileserver_cmd_file(const struct device_driver *device)
{
        /* Host receive - sector offset - 4B
         * Host Receive - sector count  - 4B
         *
         *  Per sector
         *
         * Host send    - sector        - 2048B
         * Host send    - checksum      - 1B */

        static char filename[32];

        int ret;

        if (!(_variable_read(device, filename, sizeof(filename)))) {
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
        uint32_t sector_count;

        if (!(_long_read(device, &sector_offset))) {
                verbose_printf("Error: Timed out attempting to read sector offset\n");
                /* Error */
                return;
        }

        verbose_printf("Requested sector offset: %lu\n", sector_offset);

        if (!(_long_read(device, &sector_count))) {
                /* Error */
                verbose_printf("Error: Timed out attempting to read sector count\n");
                return;
        }

        verbose_printf("Requested sector count: %lu\n", sector_count);

        for (uint32_t i = 0; i < sector_count; i++) {
                if (i >= path_entry->size) {
                        /* Error */
                        return;
                }

                const uint8_t *buffer_offset;
                buffer_offset = &path_entry->buffer[i * FILESERVER_SECTOR_SIZE];

                (void)memset(_sector_buffer, 0x00, FILESERVER_SECTOR_SIZE);
                (void)memcpy(_sector_buffer, buffer_offset, FILESERVER_SECTOR_SIZE);

                for (uint32_t try = 0; try < FILESERVER_TRIES; try++) {
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
                        if (!(_byte_read(device, &read_sector_crc))) {
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

                        device->send(FILESERVER_RET_ERROR, 1);

                        sleep(FILESERVER_SLEEP);
                }
        }

        verbose_printf("Request complete\n");
}

static void
_fileserver_cmd_size(const struct device_driver *device)
{
}

static void
_fileserver_cmd_quit(const struct device_driver *device)
{
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

        path_entry->size = (size_t)(ceilf((float)statbuf.st_size / (float)FILESERVER_SECTOR_SIZE));
        path_entry->time = statbuf.st_mtime;

        read_status_t read_status = READ_STATUS_OK;

        FILE *fp;
        if ((fp = fopen(path_entry->path, "rb")) == NULL) {
                read_status = READ_STATUS_OPEN_ERROR;
                goto error;
        }

        void *buffer;
        if ((buffer = malloc(path_entry->size * FILESERVER_SECTOR_SIZE)) == NULL) {
                fclose(fp);

                read_status = READ_STATUS_OOM;
                goto error;
        }

        (void)memset(buffer, 0x00, path_entry->size * FILESERVER_SECTOR_SIZE);

        path_entry->buffer = buffer;

        size_t size_read;
        size_read = fread(buffer, 1, statbuf.st_size, fp);

        fclose(fp);

        if (size_read != statbuf.st_size) {
                read_status = READ_STATUS_READ_ERROR;
                goto error;
        }

        crc_calculate(buffer, path_entry->size * FILESERVER_SECTOR_SIZE);

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
                        DEBUG_PRINTF("i: %i\n", i);
                        continue;
                }

                DEBUG_PRINTF("i: %i, filename: \"%s\" <?> \"%s\"\n", i, path_entry->path, filename);

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

        for (int i = 0; (dirent = readdir(dir)) != NULL; ) {
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

                verbose_printf("Serving file: \"%s\", sector count: %lu\n", path_entry->path, path_entry->size);

                if (read_status != READ_STATUS_OK) {
                        /* Error */
                        goto error;
                }

                i++;
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
