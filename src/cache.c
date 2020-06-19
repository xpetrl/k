#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>
#include <ftw.h>
#include <unistd.h>
#include <stdint.h>

#include "log.h"
#include "util.h"
#include "cache.h"
#include "pkg.h"

void cache_init(void) {
    char cwd[PATH_MAX];
    pid_t pid = getpid();

    strlcpy(HOME,    getenv("HOME"), sizeof(cwd));
    strlcpy(CAC_DIR, getenv("XDG_CACHE_HOME"), sizeof(cwd));

    SAVE_CWD;

    // XDG_CACHE_HOME is unset.
    if (CAC_DIR[0] == '\0') {
        xchdir(HOME);
        mkchdir(".cache");
        strlcpy(CAC_DIR, getcwd(cwd, sizeof(cwd)), sizeof(cwd));
    }

    mkchdir(CAC_DIR);
    mkchdir("kiss");

    strlcpy(CAC_DIR, getcwd(cwd, sizeof(cwd)), PATH_MAX + 1);
    snprintf(MAK_DIR, PATH_MAX + 22, "%s/build-%jd",   CAC_DIR, (intmax_t) pid);
    snprintf(PKG_DIR, PATH_MAX + 22, "%s/pkg-%jd",     CAC_DIR, (intmax_t) pid);
    snprintf(TAR_DIR, PATH_MAX + 22, "%s/extract-%jd", CAC_DIR, (intmax_t) pid);

    mkchdir(MAK_DIR);
    mkchdir(PKG_DIR);
    mkchdir(TAR_DIR);

    mkchdir("../sources");
    strlcpy(SRC_DIR, getcwd(cwd, sizeof(cwd)), sizeof(cwd));
    mkchdir("../bin");  
    strlcpy(BIN_DIR, getcwd(cwd, sizeof(cwd)), sizeof(cwd));

    LOAD_CWD;
}

static int rm(const char *fpath, const struct stat *sb, int tf, struct FTW *fb) {
    int rv;

    // Unused.
    (void)(sb);
    (void)(tf);
    (void)(fb);

    rv = remove(fpath);

    if (rv)
        war("Failed to remove %s", fpath);

    return rv;
}

void cache_destroy(void) {
    nftw(MAK_DIR, rm, 64, FTW_DEPTH | FTW_PHYS);
    nftw(PKG_DIR, rm, 64, FTW_DEPTH | FTW_PHYS);
    nftw(TAR_DIR, rm, 64, FTW_DEPTH | FTW_PHYS);
}