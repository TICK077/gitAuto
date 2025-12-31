#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_PATH ".git/git_auto.conf"
#define GITIGNORE_PATH ".gitignore"

/* ---------- 工具函数 ---------- */

static void trim_newline(char *s) {
    s[strcspn(s, "\r\n")] = 0;
}

static int str_eq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

/* ---------- .gitignore ---------- */

static void ensure_gitignore(void) {
    FILE *fp = fopen(GITIGNORE_PATH, "a+");
    if (!fp) return;

    char line[256];
    rewind(fp);
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        if (str_eq(line, CONFIG_PATH)) {
            fclose(fp);
            return;
        }
    }

    fprintf(fp, "\n%s\n", CONFIG_PATH);
    fclose(fp);
}

/* ---------- 配置存在 ---------- */

int config_exists(void) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) return 0;
    fclose(fp);
    return 1;
}

/* ---------- 写配置 ---------- */

int config_write(const app_config_t *cfg) {
    FILE *fp = fopen(CONFIG_PATH, "w");
    if (!fp) return -1;

    fprintf(fp,
        "remote = %s\n"
        "remote_url = %s\n"
        "branch = %s\n\n"
        "trigger = %s\n"
        "interval_minutes = %d\n"
        "exit_after_push = %s\n",
        cfg->remote,
        cfg->remote_url,
        cfg->branch,
        cfg->trigger == TRIGGER_IMMEDIATE ? "immediate" : "interval",
        cfg->interval_minutes,
        cfg->exit_after_push ? "true" : "false"
    );

    fclose(fp);
    ensure_gitignore();
    return 0;
}

/* ---------- 读配置 ---------- */

int config_load(app_config_t *cfg) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) return -1;

    char key[64], val[256];
    while (fscanf(fp, " %63[^=]= %255[^\n]\n", key, val) == 2) {
        trim_newline(val);

        if (str_eq(key, "remote"))
            strncpy(cfg->remote, val, sizeof(cfg->remote));
        else if (str_eq(key, "remote_url"))
            strncpy(cfg->remote_url, val, sizeof(cfg->remote_url));
        else if (str_eq(key, "branch"))
            strncpy(cfg->branch, val, sizeof(cfg->branch));
        else if (str_eq(key, "trigger"))
            cfg->trigger = str_eq(val, "interval")
                           ? TRIGGER_INTERVAL
                           : TRIGGER_IMMEDIATE;
        else if (str_eq(key, "interval_minutes"))
            cfg->interval_minutes = atoi(val);
        else if (str_eq(key, "exit_after_push"))
            cfg->exit_after_push = str_eq(val, "true");
    }

    fclose(fp);
    return 0;
}

/* ---------- 首次运行向导 ---------- */

int config_run_wizard(app_config_t *cfg) {
    int choice = 0;

    /* 默认值 */
    strcpy(cfg->remote, "origin");
    strcpy(cfg->branch, "main");
    cfg->interval_minutes = 5;
    cfg->exit_after_push = 1;

    printf("首次运行配置向导\n");
    printf("请输入远程仓库 URL: ");
    scanf("%255s", cfg->remote_url);

    printf("\n请选择推送触发方式:\n");
    printf("1) 打开软件即推送\n");
    printf("2) 检测到文件变动后 N 分钟推送\n");
    printf("选择 [1/2]: ");
    scanf("%d", &choice);

    if (choice == 2) {
        cfg->trigger = TRIGGER_INTERVAL;
        printf("请输入 N（分钟）: ");
        scanf("%d", &cfg->interval_minutes);
    } 
    else {
        cfg->trigger = TRIGGER_IMMEDIATE;
    }

    printf("推送完成后自动退出？[1=是 / 0=否]: ");
    scanf("%d", &cfg->exit_after_push);

    return config_write(cfg);
}
