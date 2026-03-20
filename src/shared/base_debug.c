#include "debug.h"

static FILE *debug_log_file = NULL;
static int debug_use_colors = 1;  // Set to 0 if outputting to file

void Debug_Init(FILE *log_file) {
    debug_log_file = log_file;
}

static void debug_print(const char *level_color, const char *level_str, 
                        const char *format, va_list args) {
    FILE *output = debug_log_file ? debug_log_file : stdout;

    // Get current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timebuf[20];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", tm_info);

    // Print timestamp
    fprintf(output, "[%s] ", timebuf);

    // Print level with color if enabled
    if (debug_use_colors && !debug_log_file) {
        fprintf(output, "%s", level_color);
    }
    fprintf(output, "[%s] ", level_str);
    if (debug_use_colors && !debug_log_file) {
        fprintf(output, "%s", COLOR_RESET);
    }

    // Print the actual message
    vfprintf(output, format, args);
    fprintf(output, "\n");
    fflush(output);
}

void Debug_Log(const char *format, ...) {
#if DEBUG_LEVEL >= DEBUG_ALL
    va_list args;
    va_start(args, format);
    debug_print(COLOR_RESET, "LOG", format, args);
    va_end(args);
#endif
}

void Debug_Error(const char *format, ...) {
#if DEBUG_LEVEL >= DEBUG_ERROR
    va_list args;
    va_start(args, format);
    debug_print(COLOR_RED, "ERROR", format, args);
    va_end(args);
#endif
}

void Debug_Warn(const char *format, ...) {
#if DEBUG_LEVEL >= DEBUG_WARN
    va_list args;
    va_start(args, format);
    debug_print(COLOR_YELLOW, "WARN", format, args);
    va_end(args);
#endif
}

void Debug_Info(const char *format, ...) {
#if DEBUG_LEVEL >= DEBUG_INFO
    va_list args;
    va_start(args, format);
    debug_print(COLOR_GREEN, "INFO", format, args);
    va_end(args);
#endif
}

void Debug_LogIf(int condition, const char *format, ...) {
#if DEBUG_LEVEL >= DEBUG_ALL
    if (condition) {
        va_list args;
        va_start(args, format);
        debug_print(COLOR_RESET, "LOG", format, args);
        va_end(args);
    }
#endif
}

void Debug_HexDump(const char *data, size_t len, const char *format, ...) {
#if DEBUG_LEVEL >= DEBUG_INFO
    FILE *output = debug_log_file ? debug_log_file : stdout;

    fprintf(output, "===================\n");

    va_list args;
    va_start(args, format);
    debug_print(COLOR_GREEN, "HEXDUMP", format, args);
    va_end(args);

    for (size_t i = 0; i < len; i += 16) {
        // Print offset
        fprintf(output, "%04zx: ", i);

        // Print hex values
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            fprintf(output, "%02x ", (unsigned char)data[i + j]);
            if (j == 7) fprintf(output, " ");
        }

        // Pad if necessary
        for (size_t j = len - i; j < 16; j++) {
            fprintf(output, "   ");
            if (j == 7) fprintf(output, " ");
        }

        // Print ASCII representation
        fprintf(output, " |");
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            unsigned char c = data[i + j];
            if (c >= 32 && c <= 126) {
                fprintf(output, "%c", c);
            } else {
                fprintf(output, ".");
            }
        }
        fprintf(output, "|\n");
    }
    fprintf(output, "===================\n\n");
#endif
}

void Debug_StartTimer(void) {
#ifdef _WIN32
    // Windows implementation
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    // Store in a static or thread-local variable
    //static double last_time = 0;
    //last_time = (double)count.QuadPart / freq.QuadPart;
#else
    // Unix implementation
    struct timeval tv;
    gettimeofday(&tv, NULL);
    //static double last_time = 0;
    //last_time = tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
}

double Debug_EndTimer(const char *label) {
#ifdef _WIN32
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    double current_time = (double)count.QuadPart / freq.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double current_time = tv.tv_sec + tv.tv_usec / 1000000.0;
#endif

    static double last_time = 0;
    double elapsed = current_time - last_time;
    last_time = current_time;

#if DEBUG_LEVEL >= DEBUG_INFO
    if (label) {
        Debug_Info("Timer [%s]: %.3f ms", label, elapsed * 1000.0);
    }
#endif

    return elapsed;
}
