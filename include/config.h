#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int interval_minutes;
} config_t;

void config_set_default(config_t *cfg);
int  config_load(config_t *cfg, const char *path);

#endif
