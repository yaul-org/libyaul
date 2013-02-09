/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <arpa/inet.h>

#include <ftdi.h>

#define PROG_NAME               "gdb-ftdi-proxy"

#define USB_VID                 0x0403
#define USB_PID                 0x6001
#define USB_READ_PACKET_SIZE    0x00010000
#define USB_WRITE_PACKET_SIZE   0x1000

/* Maximum buffer length for both RX and TX buffers */
#define BUF_MAXLEN              2048

struct ftdi_gdb_context {
#define FGC_OPT(fgc, opt)       ((fgc)->fgc_opts.clo_ ## opt)
        struct cmd_line_options {
                bool clo_verbose;
                bool clo_debug;
                bool clo_daemon;
                int clo_port;
                int clo_usbvid;
                int clo_usbpid;
        } fgc_opts;

        struct ftdi_context *fgc_ftdictx;
        unsigned char *fgc_txbuf;
        int fgc_sockfd;
        unsigned char *fgc_rxbuf;
};

static int process_args(struct ftdi_gdb_context *, int, char *[]);

static void init_signals(void);

static int init_ftdi(struct ftdi_gdb_context *);
static int handle_ftdi(struct ftdi_gdb_context *);
static void close_ftdi(struct ftdi_gdb_context *);

static int init_gdb(struct ftdi_gdb_context *);
static int handle_gdb(struct ftdi_gdb_context *);
static void close_gdb(struct ftdi_gdb_context *);

int
main(int argc, char *argv[])
{
        struct ftdi_gdb_context fgc;

        if ((process_args(&fgc, argc, argv)) < 0)
                return 1;

        init_signals();

        if ((init_ftdi(&fgc)) < 0)
                return 1;

        if ((init_gdb(&fgc)) < 0)
                return 1;

        while (true) {
                handle_gdb(&fgc);
                if ((handle_ftdi(&fgc)) < 0)
                        return 1;
        }

        return 0;
}

/**
 *
 */
static int
is_string_integer(const char *valstr, int *val, int base)
{
        *val = 0;

        if (valstr == NULL)
                return -1;

        char *endptr;
        long long longval;

        errno = 0;

        longval = strtoull(valstr, &endptr, base);
        if ((valstr == endptr) || (*endptr != '\0'))
                return -1;
        else if ((longval < INT_MIN) || (longval > INT_MAX)) {
                errno = ERANGE;

                return -1;
        }

        *val = (int)longval;

        return 0;
}

__attribute__ ((noreturn)) void
usage(const char *usagemsg)
{
        (void)fprintf(stderr, "%s", usagemsg);
        exit(128);
}

/**
 *
 */
static int
process_args(struct ftdi_gdb_context *fgc, int argc, char *argv[])
{
        const char *usagemsg = "Usage: " PROG_NAME
            " [-dDhv?] [-V <USB-VID>] [-P <USB-PID>] [-p <port>]\n";

        argv++;
        argc--;

        if (argc == 0)
                usage(usagemsg);

        while (argc > 0) {
                const char *argp = argv[0];

                if (((strcmp(argp, "-h")) == 0) ||
                    ((strcmp(argp, "-?")) == 0)) {
                        usage(usagemsg);
                } else if ((strcmp(argp, "-d")) == 0) {
                        FGC_OPT(fgc, debug) = true;
                } else if ((strcmp(argp, "-d")) == 0) {
                        FGC_OPT(fgc, daemon) = true;
                } else if ((strcmp(argp, "-v")) == 0) {
                        FGC_OPT(fgc, verbose) = true;
                } else if ((strcmp(argp, "-V")) == 0) {
                        argv++;
                        argc--;

                        const char *optval = argv[0];
                        int val;

                        if (optval == NULL) {
                                (void)fprintf(stderr,
                                    "-V expects a hex value\n");
                                usage(usagemsg);
                        }

                        if ((is_string_integer(
                                        optval, &val, /* base = */ 16)) < 0) {
                                (void)fprintf(stderr, "%s\n", (errno == ERANGE)
                                    ? sys_errlist[errno]
                                    : "Invalid value");

                                return -1;
                        }

                        FGC_OPT(fgc, usbvid) = val;
                } else if ((strcmp(argp, "-P")) == 0) {
                        argv++;
                        argc--;

                        const char *optval = argv[0];
                        int val;

                        if (optval == NULL) {
                                (void)fprintf(stderr,
                                    "-P expects a hex value\n");
                                usage(usagemsg);
                        }

                        if ((is_string_integer(
                                        optval, &val, /* base = */ 16)) < 0) {
                                (void)fprintf(stderr, "%s\n", (errno == ERANGE)
                                    ? sys_errlist[errno]
                                    : "Invalid integer");

                                return -1;
                        }

                        FGC_OPT(fgc, usbpid) = val;
                } else if ((strcmp(argp, "-p")) == 0) {
                        argv++;
                        argc--;

                        const char *optval = argv[0];
                        int val;


                        if (optval == NULL) {
                                (void)fprintf(stderr,
                                    "-p expects an integer value\n");
                                usage(usagemsg);
                        }

                        if ((is_string_integer(
                                        optval, &val, /* base = */ 10)) < 0) {
                                (void)fprintf(stderr, "%s\n", (errno == ERANGE)
                                    ? sys_errlist[errno]
                                    : "Invalid integer");

                                return -1;
                        }

                        FGC_OPT(fgc, port) = val;
                } else {
                        (void)fprintf(stderr, "Unknown option: %s\n", argp);
                        usage(usagemsg);
                }

                argv++;
                argc--;
        }

        return 0;
}

static void
handle_signal(int signum)
{
        switch (signum) {
        case SIGINT:
        case SIGTERM:
                return;
        }
}

/**
 * Signals that need to be installed:
 *   - SIGINT (when the user presses C-c)
 *   - SIGTERM (terminate when detaching or similar)
 */
static void
init_signals(void)
{
        struct sigaction action;
        struct sigaction old_action;

        action.sa_handler = handle_signal;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;

        sigaction(SIGINT, &action, &old_action);
        sigaction(SIGTERM, &action, &old_action);
}

/**
 *
 */
static int
init_ftdi(struct ftdi_gdb_context *fgc)
{
        struct ftdi_context *ftdi_context;
        int status;

        if ((ftdi_context = ftdi_new()) == NULL) {
                (void)fprintf(stderr, "fatal: couldn't create a new FTDI context\n");

                return -1;
        }

        if ((status = ftdi_init(ftdi_context)) < 0)
                goto error;

        status = ftdi_usb_open(ftdi_context, USB_VID, USB_PID);
        if ((status < 0) && (status != -5 /* unable to claim device */))
                goto error;

        if ((status = ftdi_usb_purge_buffers(ftdi_context)) < 0)
                goto error;

        if ((status = ftdi_read_data_set_chunksize(ftdi_context,
                    USB_READ_PACKET_SIZE)) < 0)
                goto error;

        if ((status = ftdi_write_data_set_chunksize(ftdi_context,
                    USB_WRITE_PACKET_SIZE)) < 0)
                goto error;

        if ((status = ftdi_set_bitmode(ftdi_context, 0x00,
                    BITMODE_RESET)) < 0)
                goto error;

        fgc->fgc_ftdictx = ftdi_context;

        if ((fgc->fgc_txbuf = (unsigned char *)malloc(BUF_MAXLEN)) == NULL) {
                (void)fprintf(stderr, "fatal: %s\n", sys_errlist[errno]);

                close_ftdi(fgc);

                return -1;
        }

        return 0;

error:
        (void)fprintf(stderr, "fatal: %s\n", ftdi_get_error_string(ftdi_context));
        close_ftdi(fgc);

        return -1;
}

/**
 *
 */
static int
handle_ftdi(struct ftdi_gdb_context *fgc)
{
        int len;

        memset(fgc->fgc_txbuf, 0, BUF_MAXLEN);
        len = ftdi_read_data(fgc->fgc_ftdictx, fgc->fgc_txbuf, BUF_MAXLEN);

        if (len == 0)
                return 0;

        if (len < 0) {
                /* XXX: Error */
                return -1;
        }

        if (FGC_OPT(fgc, debug)) {
                (void)printf("debug: from-FTDI: \"%s\"\n", fgc->fgc_txbuf);
        }

        /* XXX: Check if the client is still connected */

        if ((send(fgc->fgc_sockfd, fgc->fgc_txbuf, len, 0)) != len) {
                /* XXX: Error */
                return -1;
        }

        return 0;
}

/**
 *
 */
static void
close_ftdi(struct ftdi_gdb_context *fgc)
{
        if (fgc->fgc_ftdictx == NULL)
                return;

        ftdi_usb_close(fgc->fgc_ftdictx);
        ftdi_deinit(fgc->fgc_ftdictx);
        ftdi_free(fgc->fgc_ftdictx);

        fgc->fgc_ftdictx = NULL;

        free(fgc->fgc_txbuf);
}

/**
 *
 */
static int
init_gdb(struct ftdi_gdb_context *fgc)
{
        struct sockaddr_in sockaddr;
        struct sockaddr_in clnt_sockaddr;

        int sockfd;
        int clnt_sockfd;
        socklen_t clnt_addrlen;

        int reuseaddr_set;

        /* Create socket for incoming connections */
        if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                /* XXX: Error with socket(); check errno */
                return -1;
        }

        /* Force reuse of address */
        if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                    &reuseaddr_set, sizeof(reuseaddr_set))) < 0) {
                /* XXX: Error with setsockopt(); check errno */
                return -1;
        }

        /* Construct local address structure */
        memset(&sockaddr, 0, sizeof(sockaddr));

        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        sockaddr.sin_port = htons(FGC_OPT(fgc, port));

        /* Bind to the local address */
        if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
                /* XXX: Error with bind(); check errno */
                return -1;
        }

        /* Mark the socket so it will listen for incoming
         * connection(s) */
        if (listen(sockfd, /* backlog = */ 1) < 0) {
                /* XXX: Error with listen(); check errno */
                return -1;
        }

        if (FGC_OPT(fgc, debug))
                (void)printf("Listening on port %i\n", FGC_OPT(fgc, port));

        /* Set the size of the in-out parameter */
        clnt_addrlen = sizeof(clnt_sockaddr);
        /* Block waiting for the only client to connect */
        if ((clnt_sockfd = accept(sockfd, (struct sockaddr *)&clnt_sockaddr,
                    &clnt_addrlen)) < 0) {
                /* XXX: Error with accept(); check errno */
                return -1;
        }

        if ((close(sockfd)) < 0) {
                return -1;
        }

        fgc->fgc_sockfd = clnt_sockfd;

        if ((fgc->fgc_rxbuf = (unsigned char *)malloc(BUF_MAXLEN)) == NULL) {
                /* XXX: Error on allocating RX buffer */
                close_gdb(fgc);

                return -1;
        }

        return 0;
}

/**
 *
 */
static int
handle_gdb(struct ftdi_gdb_context *fgc)
{
        int len;

        memset(fgc->fgc_rxbuf, 0, BUF_MAXLEN);

        errno = 0;
        if ((len = recv(fgc->fgc_sockfd, fgc->fgc_rxbuf, BUF_MAXLEN,
                    MSG_DONTWAIT)) < 0) {
                if ((errno != EAGAIN) || (errno != EWOULDBLOCK)) {
                        /* XXX: Error with recv() */
                }

                return -1;
        }

        if (len == 0)
                return 0;

        if (FGC_OPT(fgc, debug)) {
                (void)printf("debug: from-GDB: \"%s\"\n", fgc->fgc_rxbuf);
        }

        if ((ftdi_write_data(fgc->fgc_ftdictx, fgc->fgc_rxbuf, len)) != len) {
                /* XXX: Error in writing data */
                close_ftdi(fgc);

                return -1;
        }

        return 0;
}

/**
 *
 */
static void
close_gdb(struct ftdi_gdb_context *fgc)
{
        if ((close(fgc->fgc_sockfd)) < 0)
                return;

        free(fgc->fgc_rxbuf);
}
