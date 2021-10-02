#ifndef SHARED_H
#define SHARED_H

#include <stddef.h>

typedef struct input_file {
        void * const buffer;
        size_t buffer_len;
} input_file_t;

int input_file_open(const char *file_name, input_file_t *input_file);
void input_file_close(input_file_t *input_file);

int output_file_write(const char *filename, void *buffer, size_t len);

void print_usage(const char *progname);
void print_errno(const char *progname);

#endif /* !SHARED_H */
