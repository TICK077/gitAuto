#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void config_set_default(config_t *cfg) {
    cfg->interval_minutes = 5;
}

static void trim(char *s) {
    char *p = s;
    while (*p == ' ' || *p == '\t') p++;
    memmove(s, p, strlen(p) + 1);

    size_t len = strlen(s);
    while (len && (s[len-1]=='\n'||s[len-1]=='\r'||s[len-1]==' '))
        s[--len] = 0;
}

int config_load(config_t *cfg, const char *path) {
    FILE *fp;
    char line[256];

    config_set_default(cfg);

    fp = fopen(path, "r");
    if (!fp) return -1;

    while (fgets(line, sizeof(line), fp)) {
        trim(line);
        if (!line[0] || line[0]=='#') continue;

        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = 0;
        if (strcmp(line, "interval") == 0) {
            int v = atoi(eq+1);
            if (v > 0) cfg->interval_minutes = v;
        }
    }
    fclose(fp);
    return 0;
}
