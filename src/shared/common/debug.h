#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

// Color codes for terminal output (optional)
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

// Debug levels
#define DEBUG_NONE  0
#define DEBUG_ERROR 1
#define DEBUG_WARN  2
#define DEBUG_INFO  3
#define DEBUG_ALL   4

// Set this in your build system or config
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_ALL  // Default to all debug output
#endif

// Logging functions
void Debug_Log(const char *format, ...);
void Debug_Error(const char *format, ...);
void Debug_Warn(const char *format, ...);
void Debug_Info(const char *format, ...);

// Conditional logging (only logs if condition is true)
void Debug_LogIf(int condition, const char *format, ...);

// Hex dump for debugging binary data
void Debug_HexDump(const char *data, size_t len, const char *title);

// Timing/debug info
void Debug_StartTimer(void);
double Debug_EndTimer(const char *label);

// Assert with message
#ifdef DEBUG
#define DEBUG_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            Debug_Error("Assertion failed: %s - %s (file %s, line %d)", \
                       #cond, msg, __FILE__, __LINE__); \
        } \
    } while(0)
#else
#define DEBUG_ASSERT(cond, msg) ((void)0)
#endif

#endif
