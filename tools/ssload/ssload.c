/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <ctype.h>
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "drivers.h"

#define PROGNAME "ssload"

#define VERBOSE_PRINTF(fmt, ...) do {                                          \
        if (options.v_set) {                                                   \
            (void)printf(fmt, ##__VA_ARGS__);                                  \
            (void)fflush(stdout);                                              \
        }                                                                      \
} while(false)

void console(const struct device_driver *);

static struct {
        bool h_set;
        bool v_set;

        bool x_set;
        bool u_set;
        bool d_set;
        char *filepath;

        bool i_set;
        char *iso_filepath;

        bool a_set;
        uint32_t address;

        bool s_set;
        uint32_t size;

        bool n_set;

        bool g_set;
        uint32_t port;

        bool p_set;
        uint32_t device;
} options = {
        .h_set = false,
        .v_set = false,

        .x_set = false,
        .u_set = false,
        .d_set = false,
        .filepath = NULL,

        .i_set = false,
        .iso_filepath = NULL,

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

static void
usage(void)
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
            "   -i iso\tEnable ISO9660 filesystem redirection support using image ISO\n"
            "   -g port\tStart GDB proxy on TCP port PORT\n"
            "   -p device\tChoose device DEVICE (`datalink' or `usb-cartridge')\n",
            PROGNAME);
        exit(2);
}

int
main(int argc, char **argv)
{
        const struct device_driver *device;
        device = NULL;

        int exit_code;
        exit_code = 0;

        if (argc == 1) {
                usage();
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
                        options.h_set = true;
                        break;
                case 'v':
                        options.v_set = true;
                        break;
                case 'x':
                        options.x_set = true;
                        action_cnt++;
                        options.filepath = strdup(optarg);
                        break;
                case 'u':
                        options.u_set = true;
                        action_cnt++;
                        options.filepath = strdup(optarg);
                        break;
                case 'd':
                        options.d_set = true;
                        action_cnt++;
                        options.filepath = strdup(optarg);
                        break;
                case 'a':
                        options.a_set = true;
                        options.address = (uint32_t)strtoul(optarg, &endptr, 16);
                        if (*endptr != '\0') {
                                (void)fprintf(stderr, "%s: Invalid address specified\n",
                                    PROGNAME);
                                goto error;
                        }
                        break;
                case 's':
                        options.s_set = true;
                        options.size = (uint32_t)strtoul(optarg, &endptr, 16);
                        if (*endptr != '\0') {
                                (void)fprintf(stderr, "%s: Invalid size specified\n",
                                    PROGNAME);
                                goto error;
                        }
                        break;
                case 'n':
                        options.n_set = true;
                        break;
                case 'i':
                        options.i_set = true;
                        options.iso_filepath = strdup(optarg);
                        break;
                case 'g':
                        options.g_set = true;
                        options.port = atoi(optarg);
                        break;
                case 'p':
                        options.p_set = true;
                        if ((strncmp(optarg, DEVICE_DRIVER_DATALINK_STRING,
                                    strlen(DEVICE_DRIVER_DATALINK_STRING))) == 0) {
                                options.device = DEVICE_DRIVER_DATALINK;
                        } else if ((strncmp(optarg, DEVICE_DRIVER_USB_CARTRIDGE_STRING,
                                    strlen(DEVICE_DRIVER_USB_CARTRIDGE_STRING))) == 0) {
                                options.device = DEVICE_DRIVER_USB_CARTRIDGE;
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
                                usage();
                        default:
                                (void)fprintf(stderr, "%s: Unknown option `-%c'\n",
                                    PROGNAME, optopt);
                                usage();
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
                usage();
        }

        /* Don't allow more than one action to be set */
        if (action_cnt > 1) {
                (void)fprintf(stderr, "%s: More than one type of action has been requested\n",
                    PROGNAME);
                usage();
        }

        /* Sanity checks */
        if (options.u_set) {
                if (!options.s_set) {
                        (void)fprintf(stderr, "%s: Size option `-s' needs to be specified\n",
                            PROGNAME);
                        usage();
                }
        }
        if (options.d_set) {
                if (!options.s_set) {
                        (void)fprintf(stderr, "%s: Size option `-s' needs to be specified\n",
                            PROGNAME);
                        usage();
                }
        }
        if (options.a_set) {
                if (options.address == 0) {
                        (void)fprintf(stderr, "%s: Invalid address specified\n",
                            PROGNAME);
                        goto error;
                }

                if (action_cnt == 0) {
                        (void)fprintf(stderr, "%s: One action (`-x', `-u', or `-d') needs to be specified\n",
                            PROGNAME);
                        usage();
                }
        }
        if (options.s_set) {
                if (!options.d_set) {
                        (void)fprintf(stderr, "%s: One action (`-d') needs to be specified\n",
                            PROGNAME);
                        usage();
                }
        }

        device = device_drivers[options.device];

        VERBOSE_PRINTF("Detected: %s\n", device->name);

        VERBOSE_PRINTF("Initializing...");
        if ((device->init()) < 0) {
                VERBOSE_PRINTF(" FAILED\n");
                goto error;
        }
        VERBOSE_PRINTF(" OK\n");

        if (options.x_set) {
                char *filename;
                filename = basename(options.filepath);

                VERBOSE_PRINTF("Uploading (and executing) file `%s' to 0x%08X...",
                    filename,
                    options.address);

                int ret;
                if ((ret = device->execute_file(options.filepath,
                            options.address)) < 0) {
                        VERBOSE_PRINTF(" FAILED\n");
                        goto error;
                }
                VERBOSE_PRINTF(" OK\n");
        }

        if (options.u_set) {
                char *filename;
                filename = basename(options.filepath);

                VERBOSE_PRINTF("Uploading file `%s' to 0x%08X...", filename,
                    options.address);

                int ret;
                if ((ret = device->upload_file(options.filepath,
                            options.address)) < 0) {
                        VERBOSE_PRINTF(" FAILED\n");
                        goto error;
                }
                VERBOSE_PRINTF(" OK\n");
        }

        if (options.d_set) {
                char *filename;
                filename = basename(options.filepath);

                VERBOSE_PRINTF("Downloading from 0x%08X to file `%s'...",
                    options.address,
                    filename);

                int ret;
                if ((ret = device->download_file(options.filepath, options.address,
                            options.size)) < 0) {
                        VERBOSE_PRINTF(" FAILED\n");
                        goto error;
                }
                VERBOSE_PRINTF(" OK\n");
        }

        if (options.g_set) {
                (void)fprintf(stderr, "%s: Not yet implemented\n", PROGNAME);
                exit(1);
        }

        if (options.i_set) {
                (void)fprintf(stderr, "%s: Not yet implemented\n", PROGNAME);
                exit(1);
        }

        if (!options.n_set) {
                if (options.x_set) {
                        VERBOSE_PRINTF("\n");
                        console(device);
                }
        }

        goto exit;

error:
        exit_code = 1;

exit:
        if (options.filepath != NULL) {
                free(options.filepath);
        }
        if (options.iso_filepath != NULL) {
                free(options.iso_filepath);
        }

        if (device != NULL) {
                VERBOSE_PRINTF("Shutting down...");
                device->shutdown();
                VERBOSE_PRINTF(" OK\n");
        }

        return exit_code;
}
