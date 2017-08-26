#include <cassert>
#include "headers.h"

// http://rus.har.mn/blog/2014-05-19/strtol-error-checking/
bool stringToNumber(const char *str, int *number) {
    // we need to reset `errno`
    errno = 0;

    char *endptr = nullptr;
    long num = strtol(str, &endptr, 10);

    if (errno == ERANGE) {
        switch (num) {
            case LONG_MIN:
                // underflow
                break;
            case LONG_MAX:
                // overflow
                break;
            default:
                // impossible
                assert(false);
        }
        return false;
    }

    // something else happened. die die die
    if (errno != 0) {
        return false;
    }

    // garbage at end of string
    if (*endptr != '\0') {
        return false;
    }

    *number = (int) num;
    return true;
}
