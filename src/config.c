#include "gitauto.h"
#include <stdio.h>
#include <string.h>
#include <direct.h>

config_t g_config;

static void save_default(void)
{
    FILE *fp = fopen(CONFIG_PATH, "w");
    if (!fp) return;

    fprintf(fp,
        "# gitauto config\n"
        "countdown=5\n"
        "use_whitelist=0\n"
        "watch_path=.\n"
    );
    fclose(fp);
}

void config_ensure(void)
{
    _mkdir(".git");

    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp)
        save_default();
    else
        fclose(fp);
}

void config_init(void)
{
    g_config.countdown_sec = 5;
    g_config.use_whitelist = 0;
    strcpy(g_config.watch_path, ".");

    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "countdown=%d", &g_config.countdown_sec);
        sscanf(line, "use_whitelist=%d", &g_config.use_whitelist);
        sscanf(line, "watch_path=%255s", g_config.watch_path);
    }
    fclose(fp);
}
