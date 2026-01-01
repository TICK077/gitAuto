#ifndef GITAUTO_H
#define GITAUTO_H

/* ===== 路径 ===== */
#define CONFIG_PATH ".git/gitauto.conf"
#define GITIGNORE_PATH ".gitignore"

/* ===== 配置 ===== */
typedef struct {
    int countdown_sec;
    int use_whitelist;     /* 1 白名单 / 0 黑名单 */
    char watch_path[256];
} config_t;

/* 全局配置 */
extern config_t g_config;

/* ===== config ===== */
void config_ensure(void);
void config_init(void);

/* ===== git ===== */
int git_pull(void);
int git_push_with_retry(void);

/* ===== watcher ===== */
int watch_files_changed(void);

/* ===== util ===== */
void ensure_gitignore(void);
void sleep_ms(unsigned int ms);

#endif
