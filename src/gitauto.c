#include "gitauto.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>

static Config g_cfg;
static volatile int running = 1;

/* ---------------- logging ---------------- */

static void set_color(WORD c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void log_info(const char *fmt, ...) {
    va_list ap;
    printf("[gitAuto] ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void log_warn(const char *fmt, ...) {
    set_color(FOREGROUND_RED | FOREGROUND_GREEN);
    printf("[WARN] ");
    set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void log_error(const char *fmt, ...) {
    set_color(FOREGROUND_RED);
    printf("[ERROR] ");
    set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

/* ---------------- git ---------------- */

int git_run(const char *cmd, bool quiet) {
    char buf[512];
    FILE *fp = _popen(cmd, "r");
    if (!fp) return -1;

    while (fgets(buf, sizeof(buf), fp)) {
        if (!quiet) {
            set_color(FOREGROUND_INTENSITY);
            printf("[git] %s", buf);
            set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }
    return _pclose(fp);
}

bool is_git_repo(void) {
    return system("git rev-parse --is-inside-work-tree >nul 2>nul") == 0;
}

bool has_commit(void) {
    return system("git rev-parse --verify HEAD >nul 2>nul") == 0;
}

/* ---------------- config ---------------- */

void ensure_config(void) {
    FILE *f = fopen(CONFIG_PATH, "r");
    if (f) {
        fclose(f);
        return;
    }

    log_warn("config not found, creating default");

    f = fopen(CONFIG_PATH, "w");
    fprintf(f,
        "countdown=5\n"
        "watch_whitelist=src/,include/\n"
        "watch_blacklist=.git/,build/\n");
    fclose(f);
}

void load_config(Config *cfg) {
    FILE *f = fopen(CONFIG_PATH, "r");
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "countdown=%d", &cfg->countdown) == 1) continue;
        if (sscanf(line, "watch_whitelist=%511[^\n]", cfg->whitelist) == 1) continue;
        if (sscanf(line, "watch_blacklist=%511[^\n]", cfg->blacklist) == 1) continue;
    }
    fclose(f);
}

/* ---------------- gitignore ---------------- */

void ensure_gitignore(void) {
    FILE *f = fopen(GITIGNORE_PATH, "r");
    if (f) {
        char buf[1024];
        while (fgets(buf, sizeof(buf), f)) {
            if (strstr(buf, ">>> gitAuto")) {
                fclose(f);
                return;
            }
        }
        fclose(f);
    }

    f = fopen(GITIGNORE_PATH, "a");
    fprintf(f,
        "\n# >>> gitAuto\n"
        "gitauto.exe\n"
        "# <<< gitAuto\n");
    fclose(f);
}

/* ---------------- watch ---------------- */

static BOOL watch_change(void) {
    HANDLE h = CreateFileA(
        ".",
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    char buf[2048];
    DWORD ret;
    BOOL ok = ReadDirectoryChangesW(
        h, buf, sizeof(buf), TRUE,
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_FILE_NAME,
        &ret, NULL, NULL);

    CloseHandle(h);
    return ok;
}

void watch_loop(bool quiet) {
    while (running) {
        log_info("waiting...");
        watch_change();

        int left = g_cfg.countdown;
        while (left > 0 && running) {
            log_info("change detected, countdown %d sec", left--);
            Sleep(1000);
        }

        int retry = 3;
        while (retry--) {
            int rc = git_run("git add . && git commit -m \"auto push\" && git push", quiet);
            if (rc == 0) break;
            if (retry == 0) {
                log_error("push failed");
                return;
            }
            log_warn("retry push");
        }
    }
}

/* ---------------- main ---------------- */

static void on_sig(int _) {
    running = 0;
}

int main(int argc, char **argv) {
    signal(SIGINT, on_sig);

    bool auto_mode = false;
    bool quiet = false;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-A")) auto_mode = true;
        if (!strcmp(argv[i], "-M")) auto_mode = false;
        if (!strcmp(argv[i], "--quiet")) quiet = true;
    }

    if (!is_git_repo()) {
        log_error("not a git repository");
        return 1;
    }

    ensure_config();
    ensure_gitignore();
    load_config(&g_cfg);

    if (auto_mode) {
        git_run("git pull", quiet);
        watch_loop(quiet);
    } else {
        git_run("git add . && git commit -m \"manual push\" && git push", quiet);
    }
    return 0;
}
