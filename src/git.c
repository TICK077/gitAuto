#include "gitauto.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/* ===== util ===== */

void sleep_ms(unsigned int ms)
{
    Sleep(ms);
}

void ensure_gitignore(void)
{
    FILE *fp = fopen(GITIGNORE_PATH, "a+");
    if (!fp) return;

    fprintf(fp, "\n# gitauto\nbuild/\n*.exe\n");
    fclose(fp);
}

/* ===== git ===== */

int git_pull(void)
{
    return system("git pull");
}

static int git_push_once(void)
{
    return system("git add . && git commit -m \"auto push\" && git push");
}

int git_push_with_retry(void)
{
    int r = git_push_once();
    if (r == 0)
        return 0;

    char c;
    printf("Push failed. Retry? (y/n): ");
    scanf(" %c", &c);

    if (c == 'y' || c == 'Y')
        return git_push_once();

    return -1;
}

/* ===== watcher（文件系统级） ===== */

static FILETIME last_time = {0};

static FILETIME get_dir_time(const char *path)
{
    WIN32_FILE_ATTRIBUTE_DATA info;
    GetFileAttributesExA(path, GetFileExInfoStandard, &info);
    return info.ftLastWriteTime;
}

int watch_files_changed(void)
{
    FILETIME now = get_dir_time(g_config.watch_path);

    if (CompareFileTime(&now, &last_time) != 0) {
        last_time = now;
        return 1;
    }
    return 0;
}
