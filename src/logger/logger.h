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
#include <stdbool.h>
#include <string.h>

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#ifdef PLATFORM_WINDOWS
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

static char *log_file = "default.log";
static bool log_to_console = false;
static FILE* log_stream = NULL;
static long log_max_size = 10 * 1024 * 1024; // 10MB default

static void _compress_log() {
    if (access(log_file, F_OK) != 0) return;

#ifdef HAVE_ZLIB
    char gz_name[256];
    snprintf(gz_name, sizeof(gz_name), "%s.%ld.gz", log_file, (long)time(NULL));
    
    FILE *in = fopen(log_file, "rb");
    gzFile out = gzopen(gz_name, "wb");
    
    if (in && out) {
        char buf[8192];
        int n;
        while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
            gzwrite(out, buf, n);
        }
    }
    if (in) fclose(in);
    if (out) gzclose(out);
    remove(log_file);
#else
    // Fallback: simple rename if zlib is missing
    char bak_name[256];
    snprintf(bak_name, sizeof(bak_name), "%s.%ld.bak", log_file, (long)time(NULL));
    rename(log_file, bak_name);
#endif
}

static inline void log_init(const char* filename, bool console, long max_size) {
    log_file = (char*)filename;
    log_to_console = console;
    log_max_size = max_size;
    log_stream = fopen(log_file, "a");
}

static inline void log_cleanup() {
    if (log_stream) {
        fclose(log_stream);
        log_stream = NULL;
    }
    _compress_log();
}

static inline void _log_msg(const char* level, const char* format, va_list args) {
    if (!log_stream && !log_to_console) return;

    // Check size and rotate if needed
    if (log_stream) {
        fseek(log_stream, 0, SEEK_END);
        if (ftell(log_stream) > log_max_size) {
            fclose(log_stream);
            _compress_log();
            log_stream = fopen(log_file, "a");
        }
    }

    time_t raw_time;
    struct tm* timeinfo;
    char time_buffer[20] = {0};

    time(&raw_time);
    timeinfo = localtime(&raw_time);
    if (timeinfo) {
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    }

    if (log_stream) {
        if (time_buffer[0] != '\0') { fprintf(log_stream, "[%s] ", time_buffer); }
        fprintf(log_stream, "%s : ", level);
        va_list args_copy;
        va_copy(args_copy, args);
        vfprintf(log_stream, format, args_copy);
        va_end(args_copy);
        fprintf(log_stream, "\n");
        fflush(log_stream);
    }

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
