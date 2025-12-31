#include "context.h"
#include <string.h>

void context_init(context_t *ctx) {
    memset(ctx, 0, sizeof(*ctx));
    ctx->running = 1;
}

void context_reset_runtime_flags(context_t *ctx) {
    ctx->should_push = 0;
}
