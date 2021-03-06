#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include "log.h"
#include "util.h"
#include "pkg.h"
#include "repo.h"

char **REPOS;
int REPO_LEN = 0;

void repo_init(void) {
    char *kiss_path;
    char *env;
    char *tmp;
    int i;

    env = getenv("KISS_PATH");

    if (!env || !env[0]) {
        die("KISS_PATH must be set");
    }

    kiss_path = strdup(env);

    if (!kiss_path || !kiss_path[0]) {
        die("Failed to allocate memory");
    }

    if (!strchr(kiss_path, '/')) {
        die("Invalid KISS_PATH");
    }

    /* add +1 due to inbetween count */
    /* add +1 for the fallback */
    REPO_LEN = 2 + cntchr(kiss_path, ':');
    REPOS = xmalloc(REPO_LEN * sizeof(char *));

    for (i = 0; i < REPO_LEN; i++) {
        tmp = strtok(i ? NULL : kiss_path, ":");

        /* add fallback */
        if (!tmp) {
            tmp = DB_DIR;

            /* todo: prepend KISS_ROOT */
        }

        if (tmp[0] != '/') {
            die("Repository must be absolute");
        }

        if (strlen(tmp) > PATH_MAX) {
            die("Repository exceeds PATH_MAX");
        }

        if (access(tmp, F_OK) != 0) {
            die("Repository is not accessible");
        }

        REPOS[i] = xmalloc(PATH_MAX);
        xstrlcpy(REPOS[i], tmp, PATH_MAX);
    }

    free(kiss_path);
}

void repo_destroy(void) {
    int i;

    for (i = 0; i < REPO_LEN; i++) {
        free(REPOS[i]);
    }

    free(REPOS);
}
