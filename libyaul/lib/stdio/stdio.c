#include <stdio.h>

FILE __stdin_FILE __weak;
FILE __stdout_FILE __weak;
FILE __stderr_FILE __weak;

FILE * const stdin = &__stdin_FILE;
FILE * const stdout = &__stdout_FILE;
FILE * const stderr = &__stderr_FILE;
