#include "scheduler.h"
#include "git.h"
#include "platform.h"

static long long last_change_ts = 0;

void scheduler_init(context_t *ctx) {
    (void)ctx;
    last_change_ts = 0;
}

void scheduler_update(context_t *ctx) {
    if (ctx->should_push) return;
    if (ctx->config.trigger != TRIGGER_INTERVAL) return;

    if (!git_has_changes()) {
        last_change_ts = 0;
        return;
    }

    long long now = platform_time_ms();
    if (last_change_ts == 0) {
        last_change_ts = now;
        return;
    }

    if (now - last_change_ts >=
        (long long)ctx->config.interval_minutes * 60 * 1000) {
        ctx->should_push = 1;
    }
}

void scheduler_reset(context_t *ctx) {
    ctx->should_push = 0;
    last_change_ts = 0;
}
