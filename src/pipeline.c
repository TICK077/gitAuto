#include "pipeline.h"
#include "git.h"
#include "ignore.h"
#include <stdio.h>
#include <string.h>

static int stage_precheck(context_t *ctx);
static int stage_repo(context_t *ctx);
static int stage_config(context_t *ctx);
static int stage_status(context_t *ctx);
static int stage_sync(context_t *ctx);

int pipeline_run(context_t *ctx) {
    if (stage_precheck(ctx) != 0) return -1;
    if (stage_repo(ctx)     != 0) return -2;
    if (stage_config(ctx)   != 0) return -3;
    if (stage_status(ctx)   != 0) return -4;
    if (stage_sync(ctx)     != 0) return -5;
    return 0;
}

static int stage_precheck(context_t *ctx) {
    (void)ctx;
    printf("[pipeline] PRECHECK\n");
    if (!git_exists()) {
        printf("git not found\n");
        return -1;
    }
    return 0;
}

static int stage_repo(context_t *ctx) {
    printf("[pipeline] REPO\n");
    if (!git_is_repo()) {
        git_init_repo();
        ignore_init();
    }
    return 0;
}

static int stage_config(context_t *ctx) {
    printf("[pipeline] CONFIG\n");
    if (!git_has_remote(ctx->config.remote)) {
        git_add_remote(ctx->config.remote, ctx->config.remote_url);
    }

    char cur[64];
    if (git_get_current_branch(cur, sizeof(cur)) != 0 ||
        strcmp(cur, ctx->config.branch) != 0) {
        git_checkout_branch(ctx->config.branch);
    }

    return 0;
}

static int stage_status(context_t *ctx) {
    printf("[pipeline] STATUS\n");
    ctx->has_changes = git_has_changes();
    return 0;
}

static int stage_sync(context_t *ctx) {
    printf("[pipeline] SYNC\n");

    git_pull(ctx->config.remote, ctx->config.branch);

    if (ctx->has_changes) {
        git_commit_all("auto commit");
        git_push(ctx->config.remote, ctx->config.branch);
    }

    return 0;
}
