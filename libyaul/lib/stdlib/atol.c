#include <stdlib.h>
#include <ctype.h>

long atol(const char *s)
{
        long n = 0;
        int neg = 0;

        while (isspace(*s)) {
                s++;
        }

        switch (*s) {
        case '-':
                neg = 1;
                /* Fall through */
        case '+':
                s++;
                /* Fall through */
        }

        /* Compute n as a negative number to avoid overflow on LONG_MIN */
        while (isdigit(*s)) {
                n = 10 * n - (*s++ - '0');
        }

        return neg ? n : -n;
}
