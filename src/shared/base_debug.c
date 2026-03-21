#include "base_debug.h"

static void debug_print(const char *color, const char *level, const char *format, va_list args) {
    FILE *out = stdout;

    // Get time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timebuf[16];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", tm_info);

    // Print
    fprintf(out, "[%s] %s[%s]%s ", timebuf, color, level, COLOR_RESET);
    vfprintf(out, format, args);
    fprintf(out, "\n");
    fflush(out);
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

void Debug_HexDump(const void *data, size_t len, const char *format, ...) {
#if DEBUG_LEVEL >= DEBUG_INFO
    FILE *out = stdout;

    const unsigned char *bytes = (const unsigned char *)data;

    fprintf(out, "===================\n");

    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    fprintf(out, "\n");
    va_end(args);

    for (size_t i = 0; i < len; i += 16) {
        fprintf(out, "%04zx: ", i);

        // Hex
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            fprintf(out, "%02x ", bytes[i + j]);
            if (j == 7) fprintf(out, " ");
        }

        // Padding
        size_t rem = (i + 16 > len) ? (len - i) : 16;
        for (size_t j = rem; j < 16; j++) {
            fprintf(out, "   ");
            if (j == 7) fprintf(out, " ");
        }

        // ASCII
        fprintf(out, " |");
        for (size_t j = 0; j < rem; j++) {
            unsigned char c = bytes[i + j];
            fprintf(out, "%c", (c >= 32 && c <= 126) ? c : '.');
        }
        fprintf(out, "|\n");
    }
    fprintf(out, "===================\n\n");
#endif
}
