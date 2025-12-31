#ifndef CONFIG_H
#define CONFIG_H

/* 触发策略 */
typedef enum {
    TRIGGER_IMMEDIATE = 0,   /* 启动即推送 */
    TRIGGER_INTERVAL         /* 变动后 N 分钟 */
} trigger_mode_t;

/* 配置结构 */
typedef struct {
    char remote[64];
    char remote_url[256];
    char branch[64];

    trigger_mode_t trigger;
    int interval_minutes;   /* 仅 TRIGGER_INTERVAL 有效 */
    int exit_after_push;    /* 推送后是否退出 */
} app_config_t;

/* 基本接口 */
int config_exists(void);
int config_load(app_config_t *cfg);
int config_write(const app_config_t *cfg);

/* 首次运行向导 */
int config_run_wizard(app_config_t *cfg);

#endif /* CONFIG_H */
