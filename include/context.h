#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"

/*
 * context_t:
 * 唯一运行态状态源
 */
typedef struct {
    /* === 配置 === */
    app_config_t config;

    /* === Git 状态 === */
    int has_repo;
    int has_remote;
    int has_branch;
    int has_changes;

    /* === 运行态 === */
    int should_push;
    int running;
} context_t;

/* 生命周期 */
void context_init(context_t *ctx);
void context_reset_runtime_flags(context_t *ctx);

#endif /* CONTEXT_H */
