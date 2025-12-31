// platform/win32.c
#include "platform.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void platform_sleep_ms(unsigned int ms) {
    Sleep(ms);
}

long long platform_time_ms(void) {
    return (long long)GetTickCount64();
}

void platform_console_init(void) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

#endif
