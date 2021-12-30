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

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int read_all(int fd, void *buf, size_t count, size_t *nread)
{
    char *ptr = buf;
    size_t left = count;
    ssize_t nr;
    do {
        /* limite le paramètre 'count' de read() à SSIZE_MAX */
        nr = left > 0 ? read(fd, ptr, MIN(left, SSIZE_MAX)) : 0;
        if (nr > 0) {
            ptr += nr;
            left -= nr;
        }
    } while (nr > 0 || (nr == -1 && errno == EINTR));
    *nread = count - left;
    return nr;
}

int write_all(int fd, const void *buf, size_t count, size_t *nwritten)
{
    const char *ptr = buf;
    size_t left = count;
    ssize_t nw;
    /* limite le paramètre 'count' de write() à SSIZE_MAX */
    do {
        nw = left > 0 ? write(fd, ptr, MIN(left, SSIZE_MAX)) : 0;
        if (nw > 0) {
            ptr += nw;
            left -= nw;
        }
    } while (nw > 0 || (nw == -1 && errno == EINTR));
    *nwritten = count - left;
    return nw;
}
