#ifndef GITAUTO_H
#define GITAUTO_H

#include <windows.h>
#include <stdbool.h>

#define CONFIG_PATH ".git/gitauto.conf"
#define GITIGNORE_PATH ".gitignore"

typedef struct {
    int countdown;
    char whitelist[512];
    char blacklist[512];
} Config;

/* logging */
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);

/* git */
int git_run(const char *cmd, bool quiet);
bool is_git_repo(void);
bool has_commit(void);

/* config */
void ensure_config(void);
void load_config(Config *cfg);

/* ignore */
void ensure_gitignore(void);

/* watch */
void watch_loop(bool quiet);

#endif
