#include "libkernsh-kernel.h"

int my_atoi(const char *name)
{
    int val = 0;

    for (;; name++) {
        switch (*name) {
            case '0' ... '9':
                val = 10*val+(*name-'0');
                break;
            default:
                return val;
        }
    }
}
