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
#include <stdarg.h>

#include <sys/time.h>

#include "debug.h"
#include "shared.h"
#include "drivers.h"

#define PROGNAME "ssload"

struct performance_stats {
        double ps_transfer_time;
        double ps_transfer_speed;

        uint32_t _transfer_size;
        struct timeval _time_start;
        struct timeval _time_end;
};

static struct {
        bool h_set;
        bool v_set;

        bool x_set;
        bool u_set;
        bool d_set;
        char *xud_filepath;

        bool i_set;
        char *fileserver_dirpath;

        bool a_set;
        uint32_t address;

        bool s_set;
        uint32_t size;

        bool n_set;

        bool g_set;
        uint32_t port;

        bool p_set;
        uint32_t device;
} _global_options = {
        .h_set = false,
        .v_set = false,

        .x_set = false,
        .u_set = false,
        .d_set = false,
        .xud_filepath = NULL,

        .i_set = false,
        .fileserver_dirpath = NULL,

        .a_set = false,
        .address = 0x06004000,

        .s_set = false,
        .size = 0,

        .n_set = false,

        .g_set = false,
        .port = 1234,

        .p_set = false,
        .device = DEVICE_DRIVER_DATALINK
};

static void _usage(void);

static int32_t _calculate_file_size(const char *);

static void _performance_stats_begin(struct performance_stats *, uint32_t);
static void _performance_stats_end(struct performance_stats *);

void api_handle(const struct device_driver *);

void fileserver_init(const char *);
void console_init(void);

int
main(int argc, char **argv)
{
        const struct device_driver *device;
        device = NULL;

        int exit_code;
        exit_code = 0;

        if (argc == 1) {
                _usage();
        }

        int32_t action_cnt;
        action_cnt = 0;

        char *endptr;
        endptr = NULL;

        /* Avoid having getopt() print an error message to the standard
         * error stream */
        extern int opterr;
        opterr = 0;

        int option;
        while ((option = getopt(argc, argv, "hvx:u:d:a:s:ni:g:p:")) > 0) {
                switch (option) {
                case 'h':
                        _global_options.h_set = true;
                        break;
                case 'v':
                        _global_options.v_set = true;
                        break;
                case 'x':
                        _global_options.x_set = true;
                        action_cnt++;
                        _global_options.xud_filepath = strdup(optarg);
                        break;
                case 'u':
                        _global_options.u_set = true;
                        action_cnt++;
                        _global_options.xud_filepath = strdup(optarg);
                        break;
                case 'd':
                        _global_options.d_set = true;
                        action_cnt++;
                        _global_options.xud_filepath = strdup(optarg);
                        break;
                case 'a':
                        _global_options.a_set = true;
                        _global_options.address = (uint32_t)strtoul(optarg, &endptr, 16);
                        if (*endptr != '\0') {
                                (void)fprintf(stderr, "%s: Invalid address specified\n",
                                    PROGNAME);
                                goto error;
                        }
                        break;
                case 's':
                        _global_options.s_set = true;
                        _global_options.size = (uint32_t)strtoul(optarg, &endptr, 16);
                        if (*endptr != '\0') {
                                (void)fprintf(stderr, "%s: Invalid size specified\n",
                                    PROGNAME);
                                goto error;
                        }
                        break;
                case 'n':
                        _global_options.n_set = true;
                        break;
                case 'i':
                        _global_options.i_set = true;
                        _global_options.fileserver_dirpath = strdup(optarg);
                        break;
                case 'g':
                        _global_options.g_set = true;
                        _global_options.port = atoi(optarg);
                        break;
                case 'p':
                        _global_options.p_set = true;
                        if ((strncmp(optarg, DEVICE_DRIVER_DATALINK_STRING,
                                    strlen(DEVICE_DRIVER_DATALINK_STRING))) == 0) {
                                _global_options.device = DEVICE_DRIVER_DATALINK;
                        } else if ((strncmp(optarg, DEVICE_DRIVER_USB_CARTRIDGE_STRING,
                                    strlen(DEVICE_DRIVER_USB_CARTRIDGE_STRING))) == 0) {
                                _global_options.device = DEVICE_DRIVER_USB_CARTRIDGE;
                        } else {
                                (void)fprintf(stderr, "Invalid device specified\n");
                                goto error;
                        }
                        break;
                case '?':
                        switch (optopt) {
                        case 'x':
                        case 'u':
                        case 'd':
                        case 'a':
                        case 's':
                        case 'i':
                        case 'g':
                        case 'p':
                                (void)fprintf(stderr, "%s: Missing argument for option `-%c'\n",
                                    PROGNAME, optopt);
                                _usage();
                                break;
                        default:
                                (void)fprintf(stderr, "%s: Unknown option `-%c'\n",
                                    PROGNAME, optopt);
                                _usage();
                                break;
                        }
                        break;
                default:
                        abort();
                }
        }
        /* Check for non-option arguments */
        if (optind < argc) {
                int idx;
                for (idx = optind; idx < argc; idx++) {
                        (void)fprintf(stderr, "%s: Unknown option `%s'\n",
                            PROGNAME, argv[idx]);
                }
                _usage();
        }

        /* Don't allow more than one action to be set */
        if (action_cnt < 1) {
                (void)fprintf(stderr, "%s: More than one type of action has been requested\n",
                    PROGNAME);
                _usage();
        }

        /* Sanity checks */
        if (_global_options.d_set) {
                if (!_global_options.s_set) {
                        (void)fprintf(stderr, "%s: Size option `-s' needs to be specified\n",
                            PROGNAME);
                        _usage();
                }
        }
        if (_global_options.a_set) {
                if (_global_options.address == 0) {
                        (void)fprintf(stderr, "%s: Invalid address specified\n",
                            PROGNAME);
                        goto error;
                }

                if (action_cnt == 0) {
                        (void)fprintf(stderr, "%s: One action (`-x', `-u', or `-d') needs to be specified\n",
                            PROGNAME);
                        _usage();
                }
        }
        if (_global_options.s_set) {
                if (!_global_options.d_set) {
                        (void)fprintf(stderr, "%s: Size can be set only with action `-d'\n",
                            PROGNAME);
                        _usage();
                }
        }

        device = device_drivers[_global_options.device];

        verbose_printf("Using: %s\n", device->name);

        verbose_printf("Initializing device...");
        if ((device->init()) < 0) {
                verbose_printf(" FAILED\n");
                goto error;
        }
        verbose_printf(" OK\n");

        struct performance_stats performance_stats;

        if (_global_options.x_set) {
                char *filename;
                filename = basename(_global_options.xud_filepath);

                verbose_printf("Uploading (and executing) file `%s' to 0x%08X...",
                    filename,
                    _global_options.address);

                uint32_t size;
                size = _calculate_file_size(_global_options.xud_filepath);

                int ret;
                _performance_stats_begin(&performance_stats, size); {
                        if ((ret = device->execute_file(_global_options.xud_filepath,
                                    _global_options.address)) < 0) {
                                verbose_printf(" FAILED\n");
                                goto error;
                        }
                } _performance_stats_end(&performance_stats);
                verbose_printf(" OK\n");
        }

        if (_global_options.u_set) {
                char *filename;
                filename = basename(_global_options.xud_filepath);

                verbose_printf("Uploading file `%s' to 0x%08X...", filename,
                    _global_options.address);

                uint32_t size;
                size = _calculate_file_size(_global_options.xud_filepath);

                int ret;
                _performance_stats_begin(&performance_stats, size); {
                        if ((ret = device->upload_file(_global_options.xud_filepath,
                                    _global_options.address)) < 0) {
                                verbose_printf(" FAILED\n");
                                goto error;
                        }
                } _performance_stats_end(&performance_stats);
                verbose_printf(" OK\n");
        }

        if (_global_options.d_set) {
                char *filename;
                filename = basename(_global_options.xud_filepath);

                verbose_printf("Downloading from 0x%08X to file `%s'...",
                    _global_options.address,
                    filename);

                int ret;
                _performance_stats_begin(&performance_stats, _global_options.size); {
                        if ((ret = device->download_file(_global_options.xud_filepath,
                                    _global_options.address, _global_options.size)) < 0) {
                                verbose_printf(" FAILED\n");
                                goto error;
                        }
                } _performance_stats_end(&performance_stats);
                verbose_printf(" OK\n");
        }

        if (_global_options.g_set) {
                (void)fprintf(stderr, "%s: Not yet implemented\n", PROGNAME);
                exit(1);
        }

        if (_global_options.x_set || _global_options.u_set || _global_options.d_set) {
                verbose_printf("Transfer time: %.3gs\n",
                    performance_stats.ps_transfer_time);
                verbose_printf("Transfer speed: %.3g KiB/s\n",
                    performance_stats.ps_transfer_speed);
        }

        if (_global_options.x_set &&
            (_global_options.i_set || !_global_options.n_set)) {
                if (_global_options.i_set) {
                        fileserver_init(_global_options.fileserver_dirpath);
                }

                if (!_global_options.n_set) {
                        console_init();
                }

                api_handle(device);
        }

        goto exit;

error:
        exit_code = 1;

        if (device != NULL) {
                verbose_printf("Error reported by device: %s\n",
                    device->error_stringify());
        }

exit:
        if (_global_options.xud_filepath != NULL) {
                free(_global_options.xud_filepath);
        }

        if (_global_options.fileserver_dirpath != NULL) {
                free(_global_options.fileserver_dirpath);
        }

        if (device != NULL) {
                verbose_printf("Shutting device down...");
                device->shutdown();
                verbose_printf(" OK\n");
        }

        return exit_code;
}

int
verbose_printf(const char *format, ...)
{
        if (!_global_options.v_set) {
                return 0;
        }

        va_list args;

        va_start(args, format);

        int ret;
        ret = vprintf(format, args);
        (void)fflush(stdout);

        va_end(args);

        return ret;
}

static void
_usage(void)
{
        (void)fprintf(stderr,
            "Usage %s options\n"
            " options\n"
            "   -h\t\tProgram usage\n"
            "   -v\t\tBe more verbose\n"
            "   -x filename\tUpload and execute file FILENAME\n"
            "   -u filename\tUpload file FILENAME\n"
            "   -d filename\tDownload data to FILENAME\n"
            "   -a address\tSet address to ADDRESS (base 16, default: 0x06004000)\n"
            "   -s size\tSet size to SIZE (base 16)\n"
            "   -n\t\tDisable starting console\n"
            "   -i path\tEnable filesystem redirection support\n"
            "   -g port\tStart GDB proxy on TCP port PORT\n"
            "   -p device\tChoose device DEVICE (`datalink' or `usb-cartridge')"
#ifdef DEBUG
            "\n"
            "\n"
            "Debug enabled"
#endif /* DEBUG */
            "\n",
            PROGNAME);
        exit(2);
}

static void
_performance_stats_begin(struct performance_stats *performance_stats,
    uint32_t transfer_size)
{
        gettimeofday(&performance_stats->_time_start, NULL);
        performance_stats->_transfer_size = transfer_size;
}

static void
_performance_stats_end(struct performance_stats *performance_stats)
{
        gettimeofday(&performance_stats->_time_end, NULL);

        int64_t time_end;
        time_end = (int64_t)(performance_stats->_time_end.tv_sec * 1000000LL) +
            (int64_t)performance_stats->_time_end.tv_usec;
        int64_t time_start;
        time_start = (int64_t)(performance_stats->_time_start.tv_sec * 1000000LL) +
            (int64_t)performance_stats->_time_start.tv_usec;

        double time_delta;
        time_delta = (double)(time_end - time_start) / 1000000.0;
        double transfer_size;
        transfer_size = (double)performance_stats->_transfer_size / 1024.0;

        performance_stats->ps_transfer_time = time_delta;
        performance_stats->ps_transfer_speed = transfer_size / time_delta;
}

static int32_t
_calculate_file_size(const char *input_file)
{
        int32_t size;
        size = 0;

        FILE *iffp;
        iffp = NULL;

        if ((iffp = fopen(input_file, "rb")) == NULL) {
                goto error;
        }
        if ((fseek(iffp, 0, SEEK_END)) < 0) {
                goto error;
        }
        if ((size = (int32_t)ftell(iffp)) < 0) {
                goto error;
        }

        goto exit;

error:
        size = -1;

exit:
        if (iffp != NULL) {
                fclose(iffp);
        }

        return size;
}
