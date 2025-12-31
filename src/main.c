#include "config.h"
#include "context.h"
#include "runtime.h"
#include "platform.h"

int main(void) {
    platform_console_init();
    context_t ctx;

    context_init(&ctx);

    if (!config_exists()) {
        config_run_wizard(&ctx.config);
    } else {
        config_load(&ctx.config);
    }

    runtime_init(&ctx);
    runtime_loop(&ctx);

    return 0;
}
