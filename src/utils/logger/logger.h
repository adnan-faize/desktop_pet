/**
 * @file logger.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define LOG_FILE "test.log"

bool log_to_console = false;

static inline void _log_msg(const char* level, const char* format, va_list args) {
    FILE* file_stream = fopen(LOG_FILE, "a");
    if (!file_stream && !log_to_console) { return; }

    time_t raw_time;
    struct tm* timeinfo;
    char time_buffer[20];

    time(&raw_time);
    timeinfo = localtime(&raw_time);
    if (timeinfo) {
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    }

    if (time_buffer[0] != '\0') { fprintf(file_stream, "[%s] ", time_buffer); }
    fprintf(file_stream, "%s : ", level);
    va_list args_copy;
    va_copy(args_copy, args);
    vfprintf(file_stream, format, args_copy);
    va_end(args_copy);
    fprintf(file_stream, "\n");
    fclose(file_stream);

    if (log_to_console) {
        if (time_buffer[0] != '\0') { fprintf(stdout, "[%s] ", time_buffer); }
        fprintf(stdout, "%s : ", level);
        vfprintf(stdout, format, args);
        fprintf(stdout, "\n");
        fflush(stdout);
    }
}

#ifdef DEBUG
static inline void _log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    _log_msg("DEBUG", format, args);
    va_end(args);
}
#define log_debug(...) _log_debug(__VA_ARGS__)
#else
#define log_debug(...) nullptr
#endif // DEBUG


static inline void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    _log_msg("INFO", format, args);
    va_end(args);
}

static inline void log_warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    _log_msg("WARNING", format, args);
    va_end(args);
}

static inline void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    _log_msg("ERROR", format, args);
    va_end(args);
}

static inline void log_fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    _log_msg("FATAL", format, args);
    va_end(args);
}

#endif // LOGGER_H_
