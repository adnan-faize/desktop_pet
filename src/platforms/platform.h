/**
 * @file platform.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

#if 0                                                           //----- NINTENDO 3DS
#define PLATFORM_NINTENDO_3DS
#elif 0                                                         // ----- NINTENDO NDS
#define PLATFORM_NINTENDO_NDS
#elif defined(_WIN32) || defined(_WIN64)                        // ----- WINDOWS
#define PLATFORM_WINDOWS
#elif defined(__linux__) || defined(linux) || defined(__linux)  // ----- LINUX
#define PLATFORM_LINUX
#elif defined(__APPLE__) || defined(__MACH__)                   // ----- MACOS
#define PLATFORM_MACOS
#elif 0                                                         // ----- BARE METAL
#define PLATFORM_BARE_METAL
#else                                                           // ----- ERROR
#define PLATFORM_UNKNOWN
#endif

extern bool __app_init(void);
static inline bool _app_init(void) { return __app_init(); }
extern bool __app_loop(void);
static inline bool _app_loop(void) { return __app_loop(); }
extern bool __app_exit(void);
static inline bool _app_exit(void) { return __app_exit(); }
extern bool __app_update(void);
static inline bool _app_update(void) { return __app_update(); }
