#ifndef _BASE_DEBUG_H
#define _BASE_DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// Debug levels
#define DEBUG_NONE  0
#define DEBUG_ERROR 1
#define DEBUG_WARN  2
#define DEBUG_INFO  3

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_INFO
#endif

// Platform-specific color support
#ifdef _WIN32
// Windows terminals don't always support ANSI colors, so disable them
#define COLOR_RED     ""
#define COLOR_YELLOW  ""
#define COLOR_GREEN   ""
#define COLOR_RESET   ""
#else
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"
#endif

void Debug_Error(const char *format, ...);
void Debug_Warn(const char *format, ...);
void Debug_Info(const char *format, ...);
void Debug_HexDump(const void *data, size_t len, const char *format, ...);

#endif
