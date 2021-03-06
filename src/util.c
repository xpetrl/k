#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "log.h"
#include "strlcpy.h"
#include "util.h"

void *xmalloc(size_t n) {
    void *p;

    if (n == 0) {
        die("Empty memory allocation");
    }

    p = malloc(n);

    if (!p) {
        die("Failed to allocate memory");
    }

    return p;
}

void xstrlcpy(char *dst, const char *src, size_t dsize) {
    size_t err;

    err = strlcpy(dst, src, dsize);

    if (err >= dsize) {
        die("strlcpy failed");
    }
}

void xsnprintf(char *str, size_t size, const char *fmt, ...) {
    va_list va;
    unsigned int err;

    va_start(va, fmt);
    err = vsnprintf(str, size, fmt, va);
    va_end(va);

    if (err < 1) {
        die("snprintf failed to construct string");
    }

    if (err > size) {
        die("snprintf result exceeds buffer size");
    }
}

int cntchr(const char *str, int chr) {
    const char *tmp = str;
    int i = 0;

    for (; tmp[i]; tmp[i] == chr ? i++ : *tmp++);

    return i;
}

int cntlines(FILE *file) {
    char *line = 0;
    int i = 0;

    while (getline(&line, &(size_t){0}, file) != -1) {
       i += (line[0] != '#' && line[0] != '\n');
    }

    free(line);
    rewind(file);

    return i;
}

int strsuf(const char *str, const char *suf, size_t suf_len) {
    if (!str || !suf || !suf_len) {
       return -1;
    }

    return strncmp(&str[strlen(str) - suf_len], suf, suf_len);
}

int exists_at(const char *path, const char *file, const int flags) {
    int dfd;
    int ffd;

    dfd = open(path, O_RDONLY | O_DIRECTORY);

    if (dfd == -1) {
        return 1;
    }

    ffd = openat(dfd, file, O_RDONLY | flags);
    close(dfd);

    if (ffd == -1) {
        return 1;
    }

    close(ffd);
    return 0;
}

FILE *fopenat(const char *d, const char *f, const int o, const char *m) {
    int dfd;
    int ffd;

    dfd = open(d, o | O_SEARCH);

    if (dfd == -1) {
        return NULL;
    }

    ffd = openat(dfd, f, o, 0644);
    close(dfd);

    if (ffd == -1) {
        return NULL;
    }

    /* fclose() by caller also closes the open()'d fd here */
    return fdopen(ffd, m);
}

void split_in_two(char *line, const char *delim, char **one, char **two) {
    char *tok;
    size_t len;
    int i;

    for (i = 0; i < 2; i++) {
        tok = strtok(i ? NULL : line, delim);

        if (!tok) {
            tok = "";
        }

        len = strlen(tok) + 1;

        *(i ? two : one) = xmalloc(len);
        xstrlcpy(i ? *two : *one, tok, len);
    }
}
