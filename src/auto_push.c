#include "auto_push.h"
#include "git.h"
#include "platform.h"
#include <stdio.h>

int auto_push_now(void) {
    if (!git_is_repo()) {
        printf("Not a git repository.\n");
        return -1;
    }

    if (!git_has_changes()) {
        printf("No changes detected.\n");
        return 0;
    }

    if (git_commit_auto() != 0)
        return -1;

    return git_push("origin", "main");
}

int auto_push_after_minutes(int minutes) {
    if (!git_is_repo())
        return -1;

    if (!git_has_changes())
        return 0;

    printf("Waiting %d minute(s)...\n", minutes);
    platform_sleep_ms(minutes * 60 * 1000);

    if (!git_has_changes())
        return 0;

    if (git_commit_auto() != 0)
        return -1;

    return git_push("origin", "main");
}
