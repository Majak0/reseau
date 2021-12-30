#include "utils.h"
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void vmsg(const char *prefix, const char *sep, const char *suffix,
                 const char *format, va_list ap)
{
    if (format) {
        char format2[strlen(prefix) + strlen(sep) + strlen(format) +
                     strlen(suffix) + 2];
        sprintf(format2, "%s%s%s%s\n", prefix, sep, format, suffix);
        vfprintf(stderr, format2, ap);
    } else {
        fprintf(stderr, "%s%s\n", prefix, suffix);
    }
}

static void verr(const char *format, va_list ap)
{
    int saved_errno = errno;
    vmsg("ERREUR", ": ", ".", format, ap);
    if (saved_errno != 0)
        fprintf(stderr,
                "(errno = %d: %s)\n", saved_errno, strerror(saved_errno));
}

void msg(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vmsg("# ", "", "...", format, ap);
    va_end(ap);
}

void warn(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vmsg("ATTENTION", ": ", ".", format, ap);
    va_end(ap);
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
