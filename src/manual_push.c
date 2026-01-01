#include "manual_push.h"
#include "git.h"
#include <stdio.h>

int manual_push(void) {
    if (!git_is_repo()) {
        printf("Not a git repository.\n");
        return -1;
    }

    if (!git_has_changes()) {
        printf("No changes.\n");
        return 0;
    }

    if (git_commit_auto() != 0)
        return -1;

    return git_push("origin", "main");
}
