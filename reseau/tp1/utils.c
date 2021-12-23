#include "utils.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void msg(const char *format, ...)
{
    char format2[strlen(format) + 7];
    sprintf(format2, "# %s...\n", format);
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format2, ap);
    va_end(ap);
}

static void verr(const char *format, va_list ap)
{
    int saved_errno = errno;
    if (format) {
        char format2[strlen(format) + 11];
        sprintf(format2, "ERREUR: %s.\n", format);
        vfprintf(stderr, format2, ap);
    } else {
        fprintf(stderr, "ERREUR.\n");
    }
    if (saved_errno != 0)
        fprintf(stderr,
                "(errno = %d: %s)\n", saved_errno, strerror(saved_errno));
}

void err(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    verr(format, ap);
    va_end(ap);
}

void die(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    verr(format, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}
