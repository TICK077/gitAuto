#include "gitauto.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>

static Config g_cfg;
static volatile int running = 1;
/* ---------------- build commit message ---------------- */

static void build_commit_msg(char *buf, size_t size, const char *mode)
{
    time_t t = time(NULL);
    struct tm tm_now;
    localtime_s(&tm_now, &t);

    snprintf(
        buf, size,
        "gitAuto %s push @ %04d-%02d-%02d %02d:%02d:%02d",
        mode,
        tm_now.tm_year + 1900,
        tm_now.tm_mon + 1,
        tm_now.tm_mday,
        tm_now.tm_hour,
        tm_now.tm_min,
        tm_now.tm_sec
    );
}

/* ---------------- repo init ---------------- */

int gitauto_init(void)
{
    char url[256];

    if (!is_git_repo()) {
        log_info("initializing git repository...");
        if (git_run("git init", false) != 0) {
            log_error("git init failed");
            return 1;
        }
    }

    /* 已有 commit 则不允许 init */
    if (git_run("git rev-parse --verify HEAD >nul 2>&1", true) == 0) {
        log_warn("repository already initialized");
        return 0;
    }

    printf("Enter remote repository URL: ");
    fflush(stdout);
    if (!fgets(url, sizeof(url), stdin)) {
        log_error("failed to read url");
        return 1;
    }
    url[strcspn(url, "\r\n")] = 0;

    if (url[0] == '\0') {
        log_error("empty url");
        return 1;
    }

    git_run("git branch -M main", false);

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "git remote add origin %s", url);
    git_run(cmd, false);

    git_run("git commit --allow-empty -m \"[init] initial commit\"", false);

    if (git_run("git push -u origin main", false) != 0) {
        log_error("initial push failed");
        return 1;
    }

    log_info("repository initialized");
    return 0;
}


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
        "gitAuto.exe\n"
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
            char msg[128];
            build_commit_msg(msg, sizeof(msg), "auto");
            char cmd[256];
            snprintf(
                cmd, sizeof(cmd),
                "git add . && git commit -m \"%s\" && git push",
                msg
            );
            int rc = git_run(cmd, quiet);
            if (rc == 0) break;
            if (retry == 0) {
                log_error("push failed");
                return;
            }
            log_warn("retry push");
        }
    }
}

void print_usage(void)
{
    printf(
        "gitauto - lightweight git automation tool\n"
        "\n"
        "Usage:\n"
        "  gitauto init              Initialize git repository\n"
        "  gitauto -M                Commit & push immediately\n"
        "  gitauto -A                Auto push on file changes\n"
        "  gitauto -A --quiet        Auto push with minimal output\n"
        "\n"
    );
}




/* ---------------- main ---------------- */

static void on_sig(int _) {
    running = 0;
}

int main(int argc, char **argv) {
    signal(SIGINT, on_sig);

    bool auto_mode = false;
    bool quiet = false;

    if (argc == 1) {
        print_usage();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "init")) gitauto_init();
        else if (!strcmp(argv[i], "-A")) auto_mode = true;
        else if (!strcmp(argv[i], "-M")) auto_mode = false;
        else if (!strcmp(argv[i], "--quiet")) quiet = true;
        
        else {
            printf("unknown option");
            print_usage();
            return 1;
        }
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
        char msg[128];
        build_commit_msg(msg, sizeof(msg), "manual");
        char cmd[256];
        snprintf(
            cmd, sizeof(cmd),
            "git add . && git commit -m \"%s\" && git push",
            msg
        );
        git_run(cmd, quiet);
    }
    return 0;
}
