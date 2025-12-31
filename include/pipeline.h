#ifndef PIPELINE_H
#define PIPELINE_H

#include "context.h"

/* 执行一次完整 Git 生命周期 */
int pipeline_run(context_t *ctx);

#endif /* PIPELINE_H */
