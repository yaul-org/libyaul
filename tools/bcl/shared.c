#include <sys/stat.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"

int
input_file_open(const char *file_name, input_file_t *input_file)
{
        FILE *fp;

        if ((fp = fopen(file_name, "rb")) == NULL) {
                goto exit;
        }

        struct stat in_stat;

        if ((stat(file_name, &in_stat)) != 0) {
                goto exit;
        }

        input_file->buffer_len = in_stat.st_size;

        void **buffer_p = (void **)&input_file->buffer;

        if ((*buffer_p = malloc(input_file->buffer_len)) == NULL) {
                goto exit;
        }

        if ((fread(input_file->buffer, 1, input_file->buffer_len, fp)) != input_file->buffer_len) {
                goto exit;
        }

exit:
        if (fp != NULL) {
                fclose(fp);
        }

        return errno;
}

void
input_file_close(input_file_t *input_file)
{
        free(input_file->buffer);

        const void **buffer_p = (const void **)&input_file->buffer;

        *buffer_p = NULL;

        input_file->buffer_len = 0;
}

int
output_file_write(const char *filename, void *buffer, size_t len)
{
        FILE *fp;
        fp = NULL;

        if ((fp = fopen(filename, "wb+")) == NULL) {
                goto _error;
        }

        if ((fwrite(buffer, 1, len, fp)) != len) {
                goto _error;
        }

_error:
        if (fp != NULL) {
                fclose(fp);
        }

        return errno;
}

void
print_usage(const char *progname)
{
        (void)fprintf(stderr, "Usage: %s [in-file] [out-file]\n", progname);
}

void
print_errno(const char *progname)
{
        const char * const errno_string = strerror(errno);

        (void)fprintf(stderr, "Error: %s: %s\n", progname, errno_string);
}
