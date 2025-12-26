/**
 * @file win_tray.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

#include "../platform.h"

#ifdef PLATFORM_WINDOWS
#define TRAY_SUPPORT

#include <windows.h>
#include <shellapi.h>

export bool __win_tray_init();
export bool __win_tray_update();
export bool __win_tray_exit();

#define _tray_init() nullptr
#define _tray_update() nullptr
#define _tray_exit() nullptr

#endif
