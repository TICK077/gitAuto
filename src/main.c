#include "gitauto.h"
#include <stdio.h>
#include <string.h>

static void usage(void)
{
    puts("gitauto push -A   auto push");
    puts("gitauto push -M   manual push");
}

int main(int argc, char **argv)
{
    ensure_gitignore();
    config_ensure();
    config_init();

    if (argc != 3 || strcmp(argv[1], "push") != 0) {
        usage();
        return 1;
    }

    if (strcmp(argv[2], "-M") == 0) {
        return git_push_with_retry();
    }

    if (strcmp(argv[2], "-A") == 0) {
        git_pull();
        puts("[gitauto] waiting...");

        for (;;) {
            sleep_ms(1000);

            if (!watch_files_changed())
                continue;

            printf("[gitauto] change detected, countdown %d sec\n",
                   g_config.countdown_sec);

            sleep_ms(g_config.countdown_sec * 1000);
            git_push_with_retry();

            puts("[gitauto] back to waiting...");
        }
    }

    usage();
    return 1;
}
