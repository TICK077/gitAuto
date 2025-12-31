#include "git.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 执行命令（忽略输出） */
static int run_cmd(const char *cmd) {
    return system(cmd);
}

/* 执行命令并读取一行输出 */
static int run_cmd_output(const char *cmd, char *buf, int size) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;

    if (fgets(buf, size, fp) == NULL)
        buf[0] = '\0';

    pclose(fp);
    return 0;
}

/* -------- 基础 -------- */

int git_exists(void) {
#ifdef _WIN32
    return run_cmd("git --version >nul 2>&1") == 0;
#else
    return run_cmd("git --version >/dev/null 2>&1") == 0;
#endif
}

/* -------- 仓库 -------- */

int git_is_repo(void) {
#ifdef _WIN32
    return run_cmd("git rev-parse --is-inside-work-tree >nul 2>&1") == 0;
#else
    return run_cmd("git rev-parse --is-inside-work-tree >/dev/null 2>&1") == 0;
#endif
}

int git_init_repo(void) {
    return run_cmd("git init");
}

/* -------- 远程 -------- */

int git_has_remote(const char *name) {
    char cmd[256];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd),
             "git remote get-url %s >nul 2>&1", name);
#else
    snprintf(cmd, sizeof(cmd),
             "git remote get-url %s >/dev/null 2>&1", name);
#endif
    return run_cmd(cmd) == 0;
}

int git_add_remote(const char *name, const char *url) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "git remote add %s %s", name, url);
    return run_cmd(cmd);
}

/* -------- 分支 -------- */

int git_get_current_branch(char *buf, int size) {
    if (!buf || size <= 0) return -1;

    buf[0] = '\0';
    run_cmd_output("git branch --show-current", buf, size);

    /* 去掉换行 */
    buf[strcspn(buf, "\r\n")] = 0;
    return buf[0] ? 0 : -1;
}

int git_checkout_branch(const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "git checkout %s", branch);
    return run_cmd(cmd);
}

int git_create_branch(const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "git checkout -b %s", branch);
    return run_cmd(cmd);
}

/* -------- 状态 -------- */

int git_has_changes(void) {
    FILE *fp = popen("git status --porcelain", "r");
    if (!fp) return 0;

    int changed = fgetc(fp) != EOF;
    pclose(fp);
    return changed;
}

/* -------- 同步 -------- */

int git_pull(const char *remote, const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "git pull %s %s", remote, branch);
    return run_cmd(cmd);
}

int git_commit_all(const char *msg) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "git add . && git commit -m \"%s\"", msg);
    return run_cmd(cmd);
}

int git_push(const char *remote, const char *branch) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "git push %s %s", remote, branch);
    return run_cmd(cmd);
}
