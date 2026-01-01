#include "ignore.h"
#include <stdio.h>
#include <string.h>

static const char *RULES[] = {
    "# gitauto",
    "*.exe",
    "*.log",
    "build/",
    NULL
};

static int has_line(FILE *fp, const char *line) {
    char buf[256];
    rewind(fp);
    while (fgets(buf, sizeof(buf), fp)) {
        buf[strcspn(buf, "\r\n")] = 0;
        if (!strcmp(buf, line)) return 1;
    }
    return 0;
}

int ensure_gitignore(void) {
    FILE *fp = fopen(".gitignore", "a+");
    if (!fp) return -1;

    for (int i = 0; RULES[i]; i++) {
        if (!has_line(fp, RULES[i])) {
            fseek(fp, 0, SEEK_END);
            fprintf(fp, "%s\n", RULES[i]);
        }
    }
    fclose(fp);
    return 0;
}
