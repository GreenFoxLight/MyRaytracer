#pragma once

#include <cstdint>

#define internal static
#define local_persist static
#define global_variable static

#ifdef _WIN32
#define THREAD_ENTRY_FUNC(NAME) DWORD WINAPI NAME(LPVOID Parameter)
#endif