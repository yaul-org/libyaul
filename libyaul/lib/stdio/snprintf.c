#include <stdio.h>

int
snprintf(char *__restrict str, size_t size, const char *__restrict format, ...)
{
        if (str != NULL) {
                memcpy(str, format, strlen(format) + 1);
        }

        return 0;
}
