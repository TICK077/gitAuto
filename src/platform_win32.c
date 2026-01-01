#include "platform.h"
#ifdef _WIN32
#include <windows.h>

void platform_console_init(void) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void platform_sleep_ms(unsigned int ms) {
    Sleep(ms);
}

long long platform_time_ms(void) {
    return (long long)GetTickCount64();
}
#endif
