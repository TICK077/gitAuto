#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "context.h"

/* 初始化 */
void scheduler_init(context_t *ctx);

/* 更新（每轮调用） */
void scheduler_update(context_t *ctx);

/* 推送完成后重置 */
void scheduler_reset(context_t *ctx);

#endif /* SCHEDULER_H */
