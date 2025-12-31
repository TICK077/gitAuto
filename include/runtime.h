#ifndef RUNTIME_H
#define RUNTIME_H

#include "context.h"

/* runtime 初始化 */
void runtime_init(context_t *ctx);

/* 主循环 */
void runtime_loop(context_t *ctx);

#endif /* RUNTIME_H */
