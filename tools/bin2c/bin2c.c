/*
 * Copyright (c) 2000, 2016
 * See LICENSE for details.
 *
 * Dan Potter
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define PROGNAME "bin2c"

int
main(int argc, char **argv)
{
        argc--;

        if (argc != 3) {
                (void)fprintf(stderr,
                    "Usage: %s input-file prefix output-file\n",
                    PROGNAME);
                return 2;
        }

        char *prefix;
        prefix = argv[2];

        /* Check if prefix is valid */
        size_t prefix_len;
        prefix_len = strlen(prefix);

        if (prefix_len > (26 - 5 /* _data */)) {
                (void)fprintf(stderr,
                    "%s: Error: Prefix is too long (26 characters)\n",
                    PROGNAME);
                return 1;
        }

        if ((prefix[0] != '_') && (!isalpha((int)prefix[0]))) {
                (void)fprintf(stderr,
                    "%s: Error: Invalid character in prefix\n",
                    PROGNAME);
                return 1;
        }
        uint32_t idx;
        for (idx = 1; idx < (prefix_len - 1); idx++) {
                if ((prefix[idx] == '_') || (isalnum((int)prefix[idx]))) {
                        continue;
                }

                (void)fprintf(stderr,
                    "%s: Error: Invalid character in prefix\n", PROGNAME);
                return 1;
        }

        FILE *iffp;
        if ((iffp = fopen(argv[1], "rb")) == NULL)  {
                (void)fprintf(stderr, "%s: Error: Can't open input file\n",
                    PROGNAME);
                return 1;
        }
        long left;
        fseek(iffp, 0, SEEK_END);
        left = ftell(iffp);
        fseek(iffp, 0, SEEK_SET);

        FILE *offp;
        if ((offp = fopen(argv[3], "w")) == NULL)  {
                (void)fprintf(stderr, "%s: Error: Can't open output file\n",
                    PROGNAME);
                return 1;
        }

        (void)fprintf(offp, "#include <inttypes.h>\n\n");
        (void)fprintf(offp, "const size_t %s_size = %li;\n", prefix, left);
        (void)fprintf(offp, "const uint8_t %s_data[] =", prefix);
        (void)fprintf(offp, "{\n\t");

        unsigned char buffer[2048];
        int column_cnt;

        column_cnt = 0;
        while (left > 0) {
                int read;
                read = fread(buffer, 1, 2048, iffp);
                left -= read;

                int read_idx;
                for (read_idx = 0; read_idx < read; read_idx++) {
                        char *delimeter;
                        delimeter = (left > 0) || ((read_idx + 1) != read)
                            ? ","
                            : "";

                        (void)fprintf(offp, "0x%02X%s ", buffer[read_idx],
                            delimeter);

                        if (((++column_cnt) % 8) == 0) {
                                /* 8 spaces */
                                (void)fprintf(offp, "\n%s", "        ");
                        }
                }
        }

        (void)fprintf(offp, "\n};\n");

        fclose(iffp);
        fclose(offp);

        return 0;
}
