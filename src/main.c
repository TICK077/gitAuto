#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "ignore.h"
#include "git.h"
#include "config.h"
#include "manual_push.h"
#include "auto_push.h"

static void usage(void) {
    puts("gitauto push -M   manual push");
    puts("gitauto push -A   auto push after interval");
}

int main(int argc, char **argv) {
    platform_console_init();

    if (!git_exists()) {
        printf("git not found.\n");
        return 1;
    }

    ensure_gitignore();

    if (argc < 3 || strcmp(argv[1], "push") != 0) {
        usage();
        return 1;
    }

    if (!strcmp(argv[2], "-M"))
        return manual_push();

    if (!strcmp(argv[2], "-A")) {
        config_t cfg;
        config_load(&cfg, "gitauto.conf");
        return auto_push_after_minutes(cfg.interval_minutes);
    }

    usage();
    return 1;
}
