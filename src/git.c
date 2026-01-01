#include "git.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define popen  _popen
#define pclose _pclose
#endif

static int run_cmd(const char *cmd) {
    return system(cmd) == 0 ? 0 : -1;
}

int git_exists(void) {
#ifdef _WIN32
    return run_cmd("git --version >nul 2>&1") == 0;
#else
    return run_cmd("git --version >/dev/null 2>&1") == 0;
#endif
}

int git_is_repo(void) {
#ifdef _WIN32
    return run_cmd("git rev-parse --is-inside-work-tree >nul 2>&1") == 0;
#else
    return run_cmd("git rev-parse --is-inside-work-tree >/dev/null 2>&1") == 0;
#endif
}

int git_has_changes(void) {
    FILE *fp = popen("git status --porcelain", "r");
    if (!fp) return 0;
    int c = fgetc(fp);
    pclose(fp);
    return c != EOF;
}

int git_pull(const char *remote, const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "git pull %s %s", remote, branch);
    return run_cmd(cmd);
}

int git_commit_all(const char *msg) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "git add . && git commit -m \"%s\"", msg);
    return run_cmd(cmd);
}

int git_commit_auto(void) {
    return git_commit_all("auto commit");
}

int git_push(const char *remote, const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "git push %s %s", remote, branch);
    return run_cmd(cmd);
}
