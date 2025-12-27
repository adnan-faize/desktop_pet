/**
 * @file platform.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

/* ----- NINTENDO 3DS ----- */
#if 0
#define PLATFORM_NINTENDO_3DS
#include "nintendo_3ds/3ds_app.h"

/* ----- NINTENDO NDS ----- */
#elif 0
#define PLATFORM_NINTENDO_NDS
#include "nintendo_nds/nds_app.h"

/* ----- WINDOWS ----- */
#elif defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#include "windows/win_app.h"

/* ----- LINUX ----- */
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define PLATFORM_LINUX
#include "linux/linux_app.h"

/* ----- MACOS ----- */
#elif defined(__APPLE__) || defined(__MACH__)
#define PLATFORM_MACOS
#include "macos/mac_app.h"

/* ----- BARE METAL ----- */
#elif 0
#define PLATFORM_BARE_METAL

/* ----- ERROR ----- */
#else
#define PLATFORM_UNKNOWN

#define _app_init() nullptr
#define _app_update() nullptr
#define _app_exit() nullptr

#endif
