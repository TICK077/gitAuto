#include "runtime.h"
#include "pipeline.h"
#include "scheduler.h"
#include "platform.h"

void runtime_init(context_t *ctx) {
    scheduler_init(ctx);
}

void runtime_loop(context_t *ctx) {
    /* 启动即推送 */
    if (ctx->config.trigger == TRIGGER_IMMEDIATE) {
        pipeline_run(ctx);
        return;
    }

    /* daemon 模式 */
    while (ctx->running) {
        scheduler_update(ctx);

        if (ctx->should_push) {
            pipeline_run(ctx);
            scheduler_reset(ctx);

            if (ctx->config.exit_after_push) {
                break;
            }
        }

        platform_sleep_ms(500);
    }
}